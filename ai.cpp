#include "ai.h"
#include <algorithm>
#include <cmath>

// Глобальные переменные (определение)
const int board_size = 8;
const int corner_size = 4;
std::vector<std::vector<char>> board(board_size,
                                     std::vector<char>(board_size, '.'));
std::vector<std::vector<bool>>
    inOpponentCorner(board_size, std::vector<bool>(board_size, false));
int blackMoves = 20;
int whiteMoves = 20;
std::vector<std::string> moveHistory;
size_t moveNumber = 0;

// Вспомогательные функции: isInside, isValidMove, makeMove, checkWin
bool isInside(int x, int y) {
  return x >= 0 && x < board_size && y >= 0 && y < board_size;
}

bool isValidMove(int x1, int y1, int x2, int y2, char player) {
  if (!isInside(x1, y1) || !isInside(x2, y2))
    return false;
  if (board[x1][y1] != player || board[x2][y2] != '.')
    return false;
  if (player == 'B' && inOpponentCorner[x1][y1])
    return false;

  int dx = abs(x2 - x1), dy = abs(y2 - y1);
  if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1))
    return true;
  if ((dx == 2 && dy == 0) || (dx == 0 && dy == 2)) {
    int mx = (x1 + x2) / 2, my = (y1 + y2) / 2;
    if (board[mx][my] != '.')
      return true;
  }
  return false;
}

bool makeMove(int x1, int y1, int x2, int y2, char player) {
  if (!isValidMove(x1, y1, x2, y2, player))
    return false;
  board[x2][y2] = player;
  board[x1][y1] = '.';
  if (player == 'B' && x2 <= 3 && y2 <= 3 - x2)
    inOpponentCorner[x2][y2] = true;
  return true;
}

bool checkWin(char player) {
  int count = 0;
  if (player == 'W') {
    for (int x = board_size - corner_size; x < board_size; ++x)
      for (int y = board_size - corner_size; y < board_size; ++y)
        if ((x + y) >= board_size - 1 + board_size - corner_size &&
            board[x][y] == 'W')
          ++count;
    return count >= 6;
  } else {
    for (int x = 0; x < corner_size; ++x)
      for (int y = 0; y < corner_size; ++y)
        if ((x + y) <= corner_size - 1 && board[x][y] == 'B')
          ++count;
    return count >= 6;
  }
  return false;
}

// AI функции
int distanceToCorner(int x, int y, char player) {
  int targetX = (player == 'B') ? 0 : board_size - 1;
  int targetY = (player == 'B') ? 0 : board_size - 1;
  return abs(targetX - x) + abs(targetY - y);
}

int evaluateBoard(const std::vector<std::vector<char>> &boardState,
                  int remainingBlackMoves, int remainingWhiteMoves) {
  int score = 0;
  for (int x = 0; x < board_size; ++x)
    for (int y = 0; y < board_size; ++y) {
      if (boardState[x][y] == 'B') {
        score -= distanceToCorner(x, y, 'B') * 10;
        if (x <= 3 && y <= 3 && (x + y) <= corner_size - 1)
          score += 50;
      } else if (boardState[x][y] == 'W') {
        score += distanceToCorner(x, y, 'W') * 10;
        if (x >= board_size - 4 && y >= board_size - 4 &&
            (x + y) >= 2 * board_size - corner_size - 1)
          score -= 50;
      }
    }
  if (remainingBlackMoves <= 5)
    for (int x = 0; x < corner_size; ++x)
      for (int y = 0; y < corner_size; ++y)
        if ((x + y) <= corner_size - 1 && boardState[x][y] != 'B')
          score -= 20;
  return score;
}

std::vector<Move> generateMoves(char player) {
  std::vector<Move> moves;
  const int dxs[4] = {1, -1, 0, 0};
  const int dys[4] = {0, 0, 1, -1};
  for (int x = 0; x < board_size; x++)
    for (int y = 0; y < board_size; y++)
      if (board[x][y] == player)
        for (int dir = 0; dir < 4; dir++) {
          int nx = x + dxs[dir], ny = y + dys[dir];
          if (isValidMove(x, y, nx, ny, player))
            moves.push_back({x, y, nx, ny});
          nx = x + 2 * dxs[dir];
          ny = y + 2 * dys[dir];
          if (isValidMove(x, y, nx, ny, player))
            moves.push_back({x, y, nx, ny});
        }
  return moves;
}

int minimax(int depth, bool isMaximizing, int alpha, int beta,
            int remainingBlackMoves, int remainingWhiteMoves) {
  if (depth == 0 || checkWin('B') || checkWin('W'))
    return evaluateBoard(board, remainingBlackMoves, remainingWhiteMoves);

  char player = isMaximizing ? 'B' : 'W';
  std::vector<Move> moves = generateMoves(player);
  if (moves.empty())
    return evaluateBoard(board, remainingBlackMoves, remainingWhiteMoves);

  if (isMaximizing) {
    int maxEval = -1000000;
    for (auto &m : moves) {
      auto backupBoard = board;
      auto backupCorner = inOpponentCorner;
      makeMove(m.x1, m.y1, m.x2, m.y2, player);
      int eval = minimax(depth - 1, false, alpha, beta, remainingBlackMoves - 1,
                         remainingWhiteMoves);
      board = backupBoard;
      inOpponentCorner = backupCorner;
      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
        break;
    }
    return maxEval;
  } else {
    int minEval = 1000000;
    for (auto &m : moves) {
      auto backupBoard = board;
      auto backupCorner = inOpponentCorner;
      makeMove(m.x1, m.y1, m.x2, m.y2, player);
      int eval = minimax(depth - 1, true, alpha, beta, remainingBlackMoves,
                         remainingWhiteMoves - 1);
      board = backupBoard;
      inOpponentCorner = backupCorner;
      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha)
        break;
    }
    return minEval;
  }
}

#include <chrono>

bool makeAIMove() {
  std::vector<Move> moves = generateMoves('B');
  if (moves.empty())
    return false;

  using namespace std::chrono;
  auto start = high_resolution_clock::now();
  int timeLimitMs = 500; // ограничение времени на поиск

  int bestValue = -1000000;
  Move bestMove = moves[0];

  for (auto &m : moves) {
    auto backupBoard = board;
    auto backupCorner = inOpponentCorner;

    makeMove(m.x1, m.y1, m.x2, m.y2, 'B');

    int moveValue =
        minimax(2, false, -1000000, 1000000, blackMoves - 1, whiteMoves);

    board = backupBoard;
    inOpponentCorner = backupCorner;

    if (moveValue > bestValue) {
      bestValue = moveValue;
      bestMove = m;
    }

    // Проверяем время
    auto now = high_resolution_clock::now();
    if (duration_cast<milliseconds>(now - start).count() > timeLimitMs) {
      break; // выходим, чтобы не зависнуть
    }
  }

  // Выполняем лучший найденный ход
  makeMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2, 'B');
  blackMoves--;

  char colFrom = 'A' + bestMove.y1, colTo = 'A' + bestMove.y2;
  int rowFrom = bestMove.x1 + 1, rowTo = bestMove.x2 + 1;
  moveNumber++;
  moveHistory.push_back(std::to_string(moveNumber) + ". AI: " +
                        std::string(1, colFrom) + std::to_string(rowFrom) +
                        " -> " + std::string(1, colTo) + std::to_string(rowTo));

  return true;
}
