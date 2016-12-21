#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <time.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "json.hpp"
#include "board.cpp"

using json = nlohmann::json;

struct Game {
  std::string name_cross;
  std::string name_circle;
  int winner;
  std::vector<int> moves;

  Game(std::string name_cross, std::string name_circle)
    : name_cross(name_cross),
      name_circle(name_circle),
      winner(0),
      moves(std::vector<int>())
  { }

  json to_json() {
    std::string winner_str;
    switch (winner) {
      case 1:
        winner_str = "cross";
        break;
      case 2:
        winner_str = "circle";
        break;
      case 4:
        winner_str = "tie";
        break;
      default:
        std::cerr << "unknown winner during game serialization" << std::endl;
        exit(-1);
    }

    json j = {
      {"cross", {
        {"name", name_cross}
      }},
      {"circle", {
        {"name", name_circle}
      }},
      {"winner", winner_str},
      {"moves", moves}
    };

    return j;
  }
};

struct BotHandle {
  std::string name;
  int reader;
  int writer;

  BotHandle(std::string name, int reader, int writer)
    : name(name),
      reader(reader),
      writer(writer)
  { }

  void send(std::string msg) {
    msg = std::string(msg) + "\n";
    if (write(writer, msg.c_str(), msg.length()) != (int)msg.length()) {
      std::cerr << "BotHandle write failure" << std::endl;
      exit(-1);
    }
  }

  std::string receive() {
    static const int buffer_size = 64;
    static const int sleep_ms = 64;
    char buffer[buffer_size];
    int size;
    std::string msg;

    while (true) {
      size = read(reader, buffer, buffer_size);
      switch (size) {
        case 0:
          std::cerr << "bot read handle closed unexpectedly" << std::endl;
          exit(-1);
        case -1:
          if (errno == EAGAIN) {
            if (msg.empty()) nanosleep((const struct timespec[]){{0, sleep_ms*1000}}, nullptr);
            else return msg;
          }
          else {
            std::cerr << "bot read invoked unexpected errno " << errno << std::endl;
            exit(-1);
          }
        default:
          if (size > 0) msg += std::string(buffer, size);
          break;
      }
    }
  }
};

BotHandle spawn(std::string bot_file, int log_file_handle) {
  int parent_read_pipe[2];
  int parent_write_pipe[2];
  if (pipe(parent_read_pipe) || pipe(parent_write_pipe)) {
    std::cout << "pipe failure" << std::endl;
    exit(-1);
  }

  switch (fork()) {
    case -1:
      std::cout << "fork failure" << std::endl;
      exit(-1);
    case 0: // child
      close(parent_read_pipe[0]); // close read end
      close(parent_write_pipe[1]); // close write end

      dup2(parent_write_pipe[0], STDIN_FILENO);
      dup2(parent_read_pipe[1], STDOUT_FILENO);
      dup2(log_file_handle, STDERR_FILENO);

      execl(bot_file.c_str(), bot_file.c_str(), nullptr);
      std::cerr << "bot exec failure" << std::endl;
      exit(-1);
    default: // parent
      close(parent_read_pipe[1]);  // close write end
      close(parent_write_pipe[0]); // close read end

      auto writer = parent_write_pipe[1];
      auto reader = parent_read_pipe[0];
      
      fcntl(reader, F_SETFL, O_NONBLOCK);

      int last_index;
      if ((last_index = bot_file.find_last_of("/"))  != -1 ||
          (last_index = bot_file.find_last_of("\\")) != -1)
        bot_file = bot_file.substr(last_index + 1);

      return BotHandle(bot_file, reader, writer);
  }
}

void send_settings(BotHandle bot, int bot_id) {
  bot.send("settings timebank 10000");
  bot.send("settings time_per_move 500");
  bot.send("settings player_names player1,player2");
  bot.send("settings your_bot player1");
  bot.send("settings your_botid " + std::to_string(bot_id));
}

void send_upate(BotHandle bot, Board *board, int round) {
  std::string board_str;
  for (int i = 0; i < 81; ++i)
    board_str += std::to_string(board->state[i]) + ",";

  // remove last comma
  board_str = board_str.substr(0, board_str.length() - 1);

  bot.send("update game round " + std::to_string(round));
  bot.send("update game move " + std::to_string(round));
  bot.send("update game field " + board_str);
}

std::pair<int, int> request_move(BotHandle bot) {
  bot.send("action move 10000");
  auto response = bot.receive();
  
  int x, y;
  if (sscanf(response.c_str(), "place_move %d %d", &x, &y) != 2) {
    std::cerr << "unexpected move response:" << std::endl << response << std::endl;
    exit(-1);
  }

  return std::pair<int, int>(x, y);
}

int main(int argc, char const *argv[])
{
  if (argc < 3) {
    std::cerr << "invalid arguments";
    exit(-1);
  }

  int devnull = open("/dev/null", O_WRONLY);
  auto bot1_log_file = devnull;
  auto bot2_log_file = devnull;
  if (argc >= 5) {
    bot1_log_file = open(argv[3], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    bot2_log_file = open(argv[4], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (bot1_log_file == -1 || bot2_log_file == -1) {
      std::cerr << "log file failure" << std::endl;
      exit(-1);
    }
  }

  auto bot1_file = argv[1];
  auto bot2_file = argv[2];

  Board board(std::vector<int>(81), 1, 2, 1);
  int round = 1;

  auto bot1_handle = spawn(bot1_file, bot1_log_file);
  auto bot2_handle = spawn(bot2_file, bot2_log_file);
  send_settings(bot1_handle, 1);
  send_settings(bot2_handle, 2);

  Game game(bot1_handle.name, bot2_handle.name);

  auto turn = &bot1_handle;
  while (!board.winner) {
    send_upate(*turn, &board, round);
    auto move = request_move(*turn);

    int index = Board::index(move.first, move.second);
    board.make_move(index);
    game.moves.push_back(index);

    std::cout << turn->name << ":\t" << (turn == &bot1_handle ? "X" : "O") << " " << index << std::endl;

    turn = turn == &bot1_handle
      ? &bot2_handle
      : &bot1_handle;
  }

  game.winner = board.winner;
  switch (board.winner) {
    case 1:
      std::cout << bot1_handle.name << " won!" << std::endl;
      break;
    case 2:
      std::cout << bot2_handle.name << " won!" << std::endl;
      break;
    case 4:
      std::cout << "Game tied!" << std::endl;
      break;
  }

  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  char datetime_buffer[256];
  int datetime_size = std::strftime(datetime_buffer, 256, "game-%Y-%m-%dT%H%M%S.json", &tm);
  std::string game_file_name(datetime_buffer, datetime_size);

  std::fstream game_file;
  game_file.open(game_file_name, std::fstream::out);
  game_file << game.to_json() << std::flush;
  game_file.close();

  return 0;
}