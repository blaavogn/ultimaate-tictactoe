#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>

static const unsigned int macro_zobrist_empty[]  = {
  0xfd3b5abb, 0x656dc346, 0xf66174ed, 0x8143a31f, 0x2c4ab5db, 0x973c896c, 0x18e5ce61, 0xab0e1423, 0x5453d950,
  0xceb0032f, 0xbb83cd34, 0x11bf5e9e, 0x10904dc2, 0xb07df29a, 0x009ea864, 0x9db51f7a, 0x62025bb5, 0xee87108a,
  0x98932219, 0x3800caaa, 0xbf719c63, 0x6ab4406c, 0x26ae8dce, 0xd7604d39, 0x34300234, 0x584225a1, 0xe9931b44,
  0xdaa694ce, 0x207f26d2, 0x23e92fce, 0xae7669ae, 0x9874fb4c, 0x95eee8e9, 0xaaf9b218, 0x854e06a7, 0x4b054f60,
  0x701f1019, 0x9d46c977, 0xf4ebf302, 0x7d66a095, 0x87f60961, 0x7d3eb908, 0x7e4f7b31, 0x4a3f406b, 0x6732ea60,
  0x93a1e02c, 0x923e8711, 0x225cee29, 0x63711214, 0xa1056334, 0x7ad87832, 0xc5ff9b0c, 0x23d60698, 0x3d066419,
  0xb6dfa908, 0xb1290216, 0x8e9abcf1, 0xd5d70e2f, 0x894ea554, 0xdb4a8713, 0xedfd2f21, 0x4cc03566, 0x63f8f8a3,
  0x97af4a5f, 0xba67c107, 0x9573fc1f, 0x0b5d3b8c, 0x73b336b5, 0xf6972c82, 0x730db000, 0xba538d98, 0x864bf4fb,
  0x63f116c5, 0x1449632d, 0xf57c0323, 0x3b1a7051, 0x1646c3fa, 0x4d35570f, 0x5c1ec6e0, 0xf0f935dc, 0xc336552f
};

static const unsigned int macro_zobrist_cross[]  = {
  0x0cf46381, 0x03a1a0ac, 0x0958f091, 0x54aa234a, 0x6d9996cb, 0x0eaf1055, 0x99369982, 0x73a4a478, 0x367a4a1e,
  0x17aa8d1c, 0x0f3882ca, 0xb3ed1eff, 0x0147e7d6, 0xb710a367, 0x00e789c2, 0xc741cbe0, 0x7446ee5a, 0x02e55e17,
  0x27660328, 0xd27b91c1, 0x8616a359, 0x1aadf450, 0x82f6e6f7, 0x5749ef23, 0x0724b544, 0x57b3be3d, 0x74ed5ef6,
  0x66951fb0, 0x9a86d510, 0x1710a63d, 0x4509f005, 0xff67f98e, 0x55d2bf24, 0xff38c438, 0x3c298b70, 0x30c710fa,
  0xe8a85354, 0x6dcf3611, 0xf6e97952, 0xaba94a5b, 0xe3222834, 0x756a681d, 0x68734316, 0x537a2966, 0x712c33b1,
  0x4e9dc7e4, 0x18b36e0f, 0x8ae3ca1f, 0xde7eea20, 0x67e95db0, 0xb2a23dbd, 0x8dbef3ff, 0x7d4fa920, 0xbe722bae,
  0x769a8097, 0x48578a2a, 0xe5c44626, 0x223dfdac, 0x56daa95c, 0xce6d28c9, 0x6301b4ae, 0xc9d095a4, 0x5f99e839,
  0x628bdda6, 0x2508ce86, 0xfaa3d54c, 0x049706fa, 0x3dcd8a50, 0x42976a9b, 0x41ddd112, 0x04ec1556, 0x5b514970,
  0x085d5307, 0x30d55b6e, 0x6de44b61, 0x08c1134d, 0x3702dc1d, 0xc0687041, 0x208917a9, 0xff923d59, 0x8633ce7a
};

static const unsigned int macro_zobrist_circle[] = {
  0x5fee823a, 0x5be275ff, 0x99e1a906, 0x26b329c1, 0x617cd4dc, 0x3cd28f99, 0xa9279999, 0x10f12706, 0x5f9a2b0a,
  0x4d0a8a9c, 0xe678c398, 0xa22d3e98, 0x73584e95, 0x42956534, 0xf9451656, 0xc9e8ce12, 0xf6df3182, 0x3a20d634,
  0x351591a3, 0xc11834cc, 0x70152ff8, 0x23690370, 0x8c79689f, 0xcb69826b, 0xcf3ffb2e, 0x6311aa61, 0xe043bd6a,
  0x269553c6, 0x0c570cbb, 0xefaa394e, 0xb9af25b0, 0x426d9218, 0x0500f488, 0xc4efac9e, 0x3cb3b1fd, 0xb9983c66,
  0x8981e1d7, 0xd7e60c52, 0x6fb1491b, 0xb4dc7634, 0x37988b05, 0x628e814b, 0x1a7a11fa, 0xf010ff07, 0x8daee823,
  0xb558e77c, 0x329237bf, 0xc8801d15, 0x85c3aff3, 0xdcb66651, 0x1605d283, 0x9c14b8d1, 0x1af20bad, 0x090fed69,
  0x8fcf70da, 0xda883898, 0x12f2e32a, 0xda1829bd, 0x972f3159, 0xc168ac19, 0x3fdb34ac, 0xdccf7970, 0xb8fdd99f,
  0xf13e82e5, 0x740007f1, 0x66656207, 0x08fd8dea, 0x8f5f5e8b, 0x5fca050c, 0xd09d030d, 0xd75e895d, 0x6037982b,
  0xcf844ec0, 0x39d8cfdc, 0x99b5f142, 0xb5d328f5, 0x2ee0c247, 0x4f5a254c, 0xe1e1dd47, 0x9d47db69, 0xdcdc07a0
};

class Board {
public:
  const int SELF;
  const int ENEMY;

  int turn;
  char *state;
  char *macros;
  int *macro_counts;
  int winner;

  double heuristic;
  int *macro_heuristic;

  int hash;

  Board(std::vector<int> _state, int self, int enemy, int turn)
    : SELF(self),
      ENEMY(enemy),
      turn(turn),
      state(new char[81]),
      macros(new char[9]),
      macro_counts(new int[9] {}),
      winner(0),
      heuristic(0),
      macro_heuristic(new int[9] {}),
      hash(0) {

    for (unsigned int i = 0; i < 81; ++i) {
      state[i] = _state[i];
      if (state[i]) macro_counts[macro_index(i)]++;      
    }

    for (int i = 0; i < 9; ++i)
    {
      if      (check_macro_winner(i, 1)) macros[i] = 1;
      else if (check_macro_winner(i, 2)) macros[i] = 2;
      else if (macro_counts[i] == 9)     macros[i] = MACRO_TIE;
      else                               macros[i] = MACRO_NONE;
    }

    recompute_hash();
  }

  ~Board() {
    delete [] state;
    delete [] macros;
    delete [] macro_counts;
  }

  int swap_turn() {
    if (turn == SELF)  return turn = ENEMY;
    if (turn == ENEMY) return turn = SELF;
    exit(-1);
  }

  void make_move(int index) {
    state[index] = turn;

    // update macro win state if needed
    int macro = macro_index(index);
    int count = ++macro_counts[macro];

    if (check_macro_winner(macro, turn)) {
      macros[macro] = turn;
      winner = get_winner(turn);
    }
    else if (count == 9)
      macros[macro] = MACRO_TIE;

    // update hash
    update_hash(index, STATE_EMPTY, turn);

    // update heuristic
    recompute_heuristic(macro);

    swap_turn();
  }

  void undo_move(int index) {
    // update hash
    update_hash(index, state[index], STATE_EMPTY);

    state[index] = STATE_EMPTY;

    // update macro win state
    int macro = macro_index(index);
    --macro_counts[macro];

    // this assumes make_move and undo_move
    // are used as stack operations
    macros[macro] = MACRO_NONE;
    winner = WINNER_NONE;

    recompute_macro_heuristic(macro);
    
    swap_turn();
  }

  std::vector<int> legal_moves(int prev_move) {
    int target_macro = target_macro_index(prev_move);
    std::vector<int> moves;

    if (macros[target_macro] || prev_move == -1) {
      // target macro is won/full - entire board
      for (int i = 0; i < 81; ++i)
      {
        if (state[i] || macros[macro_index(i)])
          continue;

        moves.push_back(i);
      }
    }
    else {
      // target macro is open
      int base = macro_base_index(target_macro);

      // first row
      if (!state[base])      moves.push_back(base);
      if (!state[base + 1])  moves.push_back(base + 1);
      if (!state[base + 2])  moves.push_back(base + 2);

      // second row
      if (!state[base + 9])  moves.push_back(base + 9);
      if (!state[base + 10]) moves.push_back(base + 10);
      if (!state[base + 11]) moves.push_back(base + 11);

      // third row
      if (!state[base + 18]) moves.push_back(base + 18);
      if (!state[base + 19]) moves.push_back(base + 19);
      if (!state[base + 20]) moves.push_back(base + 20);
    }

    return moves;
  }

  // TODO: Add prev_move to hash (avoid Graph History Interaction problem)
  void recompute_hash() {
    for (int i = 0; i < 81; ++i)
      xor_hash(i, state[i]);
  }

  inline void xor_hash(int index, char type) {
    switch(type) {
      case STATE_EMPTY:
        hash ^= macro_zobrist_empty[index];
        break;
      case STATE_CROSS:
        hash ^= macro_zobrist_cross[index];
        break;
      case STATE_CIRCLE:
        hash ^= macro_zobrist_circle[index];
        break;
    }
  }

  inline void update_hash(int index, char oldType, char newType) {
    xor_hash(index, oldType);
    xor_hash(index, newType);
  }

  static inline int index(int x, int y) {
    return x + y*9;
  }

  static inline std::pair<int, int> coords(int index) {
    return std::pair<int, int>(index%9, index/9);
  }

  /* Board index (0 - 80) to macro index (0 - 8) */
  static inline int macro_index(int index) {
    return index/27*3 + index%9/3;
  }

  /* 
   * Board index (0 - 80) to macro index (0 - 8)
   * corresponding to board index internal macro position.
   */
  static inline int target_macro_index(int index) {
    return index%3 + index/9%3*3;
  }

  static inline int macro_base_index(int macro) {
    return macro/3*27 + macro%3*3;
  }
  
private:
  enum Constants {
    STATE_EMPTY  = 0,
    STATE_CROSS  = 1,
    STATE_CIRCLE = 2,
    MACRO_NONE   = 0,
    MACRO_TIE    = 4,
    WINNER_NONE  = 0
  };

  bool check_macro_winner(int macro, int color) {
    int base = macro_base_index(macro);

    return
      // horizontals
      (state[base]      & state[base + 1]  & state[base + 2] ) == color ||
      (state[base + 9]  & state[base + 10] & state[base + 11]) == color ||
      (state[base + 18] & state[base + 19] & state[base + 20]) == color ||

      // verticals
      (state[base]      & state[base + 9]  & state[base + 18]) == color ||
      (state[base + 1]  & state[base + 10] & state[base + 19]) == color ||
      (state[base + 2]  & state[base + 11] & state[base + 20]) == color ||

      // diagonals
      (state[base]      & state[base + 10] & state[base + 20]) == color ||
      (state[base + 2]  & state[base + 10] & state[base + 18]) == color;
  }

  int get_winner(int changed_color) {
    bool changed_won =
      // horizontals
      (macros[0] & macros[1] & macros[2]) == changed_color ||
      (macros[3] & macros[4] & macros[5]) == changed_color ||
      (macros[6] & macros[7] & macros[8]) == changed_color ||

      // verticals
      (macros[0] & macros[3] & macros[6]) == changed_color ||
      (macros[1] & macros[4] & macros[7]) == changed_color ||
      (macros[2] & macros[5] & macros[8]) == changed_color ||

      // diagonals
      (macros[0] & macros[4] & macros[8]) == changed_color ||
      (macros[2] & macros[4] & macros[6]) == changed_color;

    if (changed_won) return changed_color;

    int closed_macros = 0;
    while (macros[closed_macros++]);
    return closed_macros == 9 ? MACRO_TIE : MACRO_NONE;
  }

  inline int macro_h0_line_value(int macro_base, int color) {
    return macro_line_value(macro_base, macro_base + 1, macro_base + 2, color);
  }
  
  inline int macro_h1_line_value(int macro_base, int color) {
    return macro_line_value(macro_base + 9, macro_base + 10, macro_base + 11, color);
  }
  
  inline int macro_h2_line_value(int macro_base, int color) {
    return macro_line_value(macro_base + 18, macro_base + 19, macro_base + 20, color);
  }

  inline int macro_v0_line_value(int macro_base, int color) {
    return macro_line_value(macro_base, macro_base + 9, macro_base + 18, color);
  }

  inline int macro_v1_line_value(int macro_base, int color) {
    return macro_line_value(macro_base + 1, macro_base + 10, macro_base + 19, color);
  }

  inline int macro_v2_line_value(int macro_base, int color) {
    return macro_line_value(macro_base + 2, macro_base + 11, macro_base + 20, color);
  }

  inline int macro_d0_line_value(int macro_base, int color) {
    return macro_line_value(macro_base, macro_base + 10, macro_base + 20, color);
  }

  inline int macro_d1_line_value(int macro_base, int color) {
    return macro_line_value(macro_base + 2, macro_base + 10, macro_base + 18, color);
  }

  inline int macro_line_value(int a, int b, int c, int color) {
    if ((state[a] | state[b] | state[c]) != color)
      return 0; // empty or unwinable line

    int value = 0;
    if (state[a] == color) value++;
    if (state[b] == color) value++;
    if (state[c] == color) value++;
    return value*value;
  }

  void recompute_macro_heuristic(int macro) {
    if (macros[macro] == SELF) {
      macro_heuristic[macro] = 30;
      return;
    }
    else if (macros[macro] == ENEMY) {
      macro_heuristic[macro] = -30;
      return;
    }

    int base = macro_base_index(macro);
    macro_heuristic[macro] =
      macro_h0_line_value(base, SELF) + macro_h1_line_value(base, SELF) + macro_h2_line_value(base, SELF) +
      macro_v0_line_value(base, SELF) + macro_v1_line_value(base, SELF) + macro_v2_line_value(base, SELF) +
      macro_d0_line_value(base, SELF) + macro_d1_line_value(base, SELF) -
      
      macro_h0_line_value(base, ENEMY) - macro_h1_line_value(base, ENEMY) - macro_h2_line_value(base, ENEMY) -
      macro_v0_line_value(base, ENEMY) - macro_v1_line_value(base, ENEMY) - macro_v2_line_value(base, ENEMY) -
      macro_d0_line_value(base, ENEMY) - macro_d1_line_value(base, ENEMY);
  }

  void recompute_macro_heuristic_all() {
    for (int i = 0; i < 9; ++i)
      recompute_macro_heuristic(i);
  }

  void recompute_heuristic(int macro) {
    recompute_macro_heuristic(macro);
  
    double h0 = heuristic_macro_line(0, 1, 2);
    double h1 = heuristic_macro_line(3, 4, 5);
    double h2 = heuristic_macro_line(6, 7, 8);

    double v0 = heuristic_macro_line(0, 3, 6);
    double v1 = heuristic_macro_line(1, 4, 7);
    double v2 = heuristic_macro_line(2, 5, 8);

    double d0 = heuristic_macro_line(0, 4, 8);
    double d1 = heuristic_macro_line(2, 4, 6);

    heuristic = h0 + h1 + h2 + v0 + v1 + v2 + d0 + d1;
  }

  double heuristic_macro_line(int a, int b, int c) {
    double ha = macro_heuristic[a];
    double hb = macro_heuristic[b];
    double hc = macro_heuristic[c];

    if ((ha == 30 || hb == 30 || hc == 30) && (ha == -30 || hb == -30 || hc == -30)) {
      // unwinable line
      return 0;
    }

    return (ha + hb + hc)*2;
  }
};
