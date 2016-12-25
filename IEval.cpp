#pragma once

class IEval{
	protected:	
		const int ND = 0;
		const int X = 1;
		const int O = 2;
		const int DR = 3;

	public:
		float H(char *board, char *lmBoard, char pl, TicTacEval *ticTacEval);
};