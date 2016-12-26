#pragma once
#include <iostream>

class Printer{			
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	public:
		void PrintMac(char* c){
			for(int i = 0; i < 9; i++){
				if(i % 3 == 0){
					fprintf(stderr, "\n");
				}
				fprintf(stderr, "%d,", c[i]);
			}
			fprintf(stderr, "\n");
		}

		void Print(char* board){
			fprintf( stderr, "  012 345 678 \n");
			int row = 0;
			for(char k = 0; k < 9; k++){
				if(k%3 == 0){
					fprintf( stderr, " |-----------|\n%d|", row);
					row++;
				}
				for(char i = 0; i < 3; i++){
					for(char j = 0; j < 3; j++){
						char p = board[(k / 3) * 18 + k * 3 + i * 9 + j];

						char c = ' ';
						if(p == O){ c = 'O';}
						if(p == X){ c = 'X';}
						fprintf( stderr, "%c", c);
					}
					fprintf( stderr, "|");
				}
				if(k%3 != 2){
					fprintf( stderr, "\n%d|", row);
					row++;
				}else{
					fprintf( stderr, "\n");
				}
			}
			fprintf( stderr, (" |-----------|\n"));

			// fprintf( stderr, " [");
			// for(char k = 0; k < 9; k++){
				
			// 	for(char i = 0; i < 3; i++){
			// 		for(char j = 0; j < 3; j++){
			// 			char p = board[(k / 3) * 18 + k * 3 + i * 9 + j];
			// 			char c = 0;
			// 			if(p == 2){ c = 2;}
			// 			if(p == 1){ c = 1;}
			// 			fprintf( stderr, "%d,", c);
			// 		}
			// 	}
			// }
			// fprintf( stderr, (" ]\n\n"));
		}
		};