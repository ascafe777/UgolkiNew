#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

const int board_size = 8;
const int corner_size = 3;
char board[board_size][board_size];

void initBoard() {
    for (int i = 0; i < board_size; i++)
        for (int j = 0; j < board_size; j++)
            board[i][j] = '.';

    for (int i = 0; i < corner_size; ++i)
        for (int j = 0; j < corner_size - i; ++j)
            board[i][j] = 'W';

    for (int i = 0; i < corner_size; ++i)
        for (int j = 0; j < corner_size - i; ++j)
            board[board_size - 1 - i][board_size - 1 - j] = 'B';
}

TEST_CASE("initBoard sets up the board correctly") {
    initBoard();

    // Проверяем белые фишки
    for (int i = 0; i < corner_size; ++i)
        for (int j = 0; j < corner_size - i; ++j)
            CHECK(board[i][j] == 'W');

    // Проверяем черные фишки
    for (int i = 0; i < corner_size; ++i)
        for (int j = 0; j < corner_size - i; ++j)
            CHECK(board[board_size - 1 - i][board_size - 1 - j] == 'B');

  // Проверяем пустые клетки
	for (int i = 0; i < board_size; ++i) {
		for (int j = 0; j < board_size; ++j) {
			// верхний левый угол (белый треугольник)
			bool in_white_corner = (i < corner_size && j < corner_size - i);

			// нижний правый угол (черный треугольник)
			bool in_black_corner = false;
			for (int bi = 0; bi < corner_size; ++bi)
				for (int bj = 0; bj < corner_size - bi; ++bj)
					if (i == board_size - 1 - bi && j == board_size - 1 - bj)
						in_black_corner = true;

			if (!in_white_corner && !in_black_corner)
				CHECK(board[i][j] == '.');
		}
	}

}
