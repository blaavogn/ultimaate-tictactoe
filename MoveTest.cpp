#include <iostream>
#include <vector>
#include "TicTacEval.cpp"
#include "Printer.cpp"
#include "Mover.cpp"

class MoveTest{
	Printer *printer;
	TicTacEval *ticTacEval;
	Mover *mover; 

	int *moves;
	char *board;
	uint64_t *mBoardFull;
	char *mBoard;
	int *plMoves;
	int *opMoves;
	
	public:
		MoveTest(std::vector<char*> *inBoards){
			printer = new Printer();
			ticTacEval = new TicTacEval();
			moves = new int[81];
			board = new char[81];
			plMoves = new int[82 * 2];
			opMoves = new int[82 * 2];
			
			mBoardFull = new uint64_t[9];

			for(int i = 0; i < 81; i++){
				moves[i] = i;
				board[i] = 0;
			}

			for(int i = 0; i < 9; i++){
				mBoardFull[i] = 0;
				mBoard[i] = 0;
			}
			mover = new Mover(board, mBoardFull, mBoard, moves);

			for(auto it = inBoards->begin(); it != inBoards->end(); ++it){
				for(int i = 0; i < 81; i++){
					board[i] = (*it)[i];
				}
				
				for(int i = 0; i < 9; i++){
					mBoardFull[i] = ticTacEval->eval(board + i * 9);
					mBoard[i] = mBoardFull[i] & TicTacEval::BM_EVAL;
				}

				fprintf(stderr, "\n");
				test(-1 , -1);
				fprintf(stderr, "\n");
				test(3 ,40);
				fprintf(stderr, "\n");
				test(55, 13);
				fprintf(stderr, "\n");
				test(12, 31);
				fprintf(stderr, "\n");
				printer->Print(board);
			}
		}

		void test(int lastMove, int prefMove){
			fprintf(stderr, "Last move: %d | Prefered: %d\n", lastMove, prefMove);
			int qMovePl = 0, qMoveOp = 0;
			int qMovePlLow = 0, qMoveOpLow = 0;
			mover->getMoves(plMoves, lastMove, 1, &qMovePl, &qMovePlLow, prefMove);
			fprintf(stderr, "Pl\n");
			printMoves(plMoves, qMovePl, qMovePlLow);
			mover->getMoves(opMoves, lastMove, 2, &qMoveOp, &qMoveOpLow, prefMove);
			fprintf(stderr, "Op\n");
			printMoves(opMoves, qMoveOp, qMoveOpLow);
		}

		void printMoves(int* moves, int qmoves, int qLow){
			fprintf(stderr, "Q: %d, low: %d\n", qmoves, qLow);
			for(int i = 0; i < 81; i++){
				if(moves[i] == 999){
					break;
				}
				fprintf(stderr, "%d,", moves[i]);
			}
			fprintf(stderr, "\n");
		}
};

int main ()
{
  std::string input_line;
	std::vector<std::string> *lines = new std::vector<std::string>();
	std::vector<char*> *allBoards = new std::vector<char*>();

	while (getline(std::cin, input_line)) {
  	lines->push_back(input_line);
  };

	for(auto it = lines->begin(); it != lines->end(); ++it ){
		char *tmpBoardPl = new char[81];
		char *tmpBoardOp = new char[81];

		for(int i = 0; i < 81; i++){
			char v = (*it).at(i * 2);
			tmpBoardPl[i] = (v == '0' ? 0 : (v == '1' ? 1 : 2));
			tmpBoardOp[i] = (v == '0' ? 0 : (v == '1' ? 2 : 1));
		}
		allBoards->push_back(tmpBoardPl);
		allBoards->push_back(tmpBoardOp);
	}

	new MoveTest(allBoards);

	return 0;
}