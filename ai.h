#pragma once
#include <string>
#include <vector>

struct Move {
  int x1, y1, x2, y2;
};

// Основные функции AI
bool makeAIMove();
std::vector<Move> generateMoves(char player);
int minimax(int depth, bool isMaximizing, int alpha, int beta,
            int remainingBlackMoves, int remainingWhiteMoves);
int evaluateBoard(const std::vector<std::vector<char>> &boardState,
                  int remainingBlackMoves, int remainingWhiteMoves);
int distanceToCorner(int x, int y, char player);

// Вспомогательные функции
bool isInside(int x, int y);
bool isValidMove(int x1, int y1, int x2, int y2, char player);
bool makeMove(int x1, int y1, int x2, int y2, char player);
bool checkWin(char player);

// Глобальные переменные (extern)
extern const int board_size;
extern const int corner_size;
extern std::vector<std::vector<char>> board;
extern std::vector<std::vector<bool>> inOpponentCorner;
extern int blackMoves;
extern int whiteMoves;
extern std::vector<std::string> moveHistory;
extern size_t moveNumber;
