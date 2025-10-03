/**
 * @file main.cpp
 * @brief Основной файл проекта "UGOLKI".
 * 
 * Содержит реализацию игрового окна, меню, доски, хода игрока и ИИ.
 */

#include <SFML/Graphics.hpp>
#include <windows.h>
#include <string>
#include "ai.h"

const int CORNER_SIZE = 4;
const int cell_size = 160;
const int border = 80;
const int history_width = 320;
const int window_size = board_size * cell_size + border * 2;
const int window_width = window_size + history_width;
const int window_heighT = board_size * cell_size + border * 2;
int scrollOffset = 0; // сдвиг для прокрутки списка ходов
const int max_visible_moves = 50; // максимум отображаемых ходов
int selectedX = -1, selectedY = -1;
bool pieceSelected = false;
bool playerTurn = true;

sf::Font font;
sf::Texture boardBackgroundTexture;
sf::Sprite boardBackground;

/**
 * @brief Инициализирует игровую доску.
 * 
 * Полностью очищает доску и расставляет начальные фишки
 * белого и черного цвета.
 */

void initBoard() {
    // полностью очищаем доску
    for (int i = 0; i < board_size; i++)
        for (int j = 0; j < board_size; j++)
            board[i][j] = '.';

    // ставим белые фишки
    for (int i = 0; i < CORNER_SIZE; ++i)
        for (int j = 0; j < CORNER_SIZE - i; ++j)
            board[i][j] = 'W';

    // ставим черные фишки
    for (int i = 0; i < CORNER_SIZE; ++i)
        for (int j = 0; j < CORNER_SIZE - i; ++j)
            board[board_size - 1 - i][board_size - 1 - j] = 'B';
}

/**
 * @brief Отрисовывает игровую доску и фигуры.
 * 
 * @param window Ссылка на окно SFML для отрисовки.
 * @param selectedX Координата X выбранной клетки.
 * @param selectedY Координата Y выбранной клетки.
 * @param pieceSelected Флаг, выбран ли игроком элемент.
 * 
 * Отображает клетки, фигуры, подсветку выбранной клетки и историю ходов.
 */
void drawBoard(sf::RenderWindow &window,int selectedX,int selectedY,bool pieceSelected){
    sf::RectangleShape cell(sf::Vector2f(cell_size-2.f,cell_size-2.f));
    for(int i = 0;i < board_size; i++)
        for(int j = 0; j < board_size; j++){
            cell.setPosition(border+j*cell_size+1.f,border+(board_size-1-i)*cell_size+1.f);
            cell.setFillColor((i+j) % 2 == 0?sf::Color(220,220,220):sf::Color(180,180,180));

			//обводка
			cell.setOutlineThickness(2.f);// толщина рамки
			cell.setOutlineColor(sf::Color::Black); // цвет рамки

            window.draw(cell);
            if(board[i][j]=='W'||board[i][j]=='B'){
                sf::CircleShape piece(cell_size/2.f-20.f);
                piece.setPosition(border+j*cell_size+20.f,border+(board_size-1-i)*cell_size+20.f);
                piece.setFillColor(board[i][j]=='W'?sf::Color::White:sf::Color::Black);
                window.draw(piece);
            }
        }
    if(pieceSelected&&isInside(selectedX,selectedY)){
        sf::RectangleShape hl(sf::Vector2f(cell_size-2.f,cell_size-2.f));
        hl.setPosition(border+selectedY*cell_size+1.f,border+(board_size-1-selectedX)*cell_size+1.f);
        hl.setFillColor(sf::Color(255,255,0,100));
        window.draw(hl);
    }

    sf::Text coord, moveText;
    coord.setFont(font); coord.setCharacterSize(24); coord.setFillColor(sf::Color::Black);
    moveText.setFont(font); moveText.setCharacterSize(20); moveText.setFillColor(sf::Color::Black);

    for(int i=0;i<board_size;i++){
        char c='A'+i;
        coord.setString(c);
        coord.setPosition(border+i*cell_size+cell_size/2.f-12.f,border-30.f); window.draw(coord);
        coord.setPosition(border+i*cell_size+cell_size/2.f-12.f,border+board_size*cell_size+5.f); window.draw(coord);
        coord.setString(std::to_string(i+1));
        coord.setPosition(border-30.f,border+(board_size-1-i)*cell_size+cell_size/2.f-12.f); window.draw(coord);
        coord.setPosition(border+board_size*cell_size+5.f,border+(board_size-1-i)*cell_size+cell_size/2.f-12.f); window.draw(coord);
    }

	// вывод истории ходов с прокруткой
	int totalMoves = static_cast<int>(moveHistory.size()); // убрали warning
	int start = std::max(0, totalMoves - max_visible_moves - scrollOffset);
	int end   = std::min(totalMoves, start + max_visible_moves);

	for (int i = start; i < end; i++) {
		moveText.setString(moveHistory[i]);
		moveText.setPosition(window_size + 20.f, border + (i - start) * 25.f);
		window.draw(moveText);
	}

}
/**
 * @enum MenuState
 * @brief Состояния главного меню.
 */
enum class MenuState { MAIN, RULES };
/**
 * @brief Отображает главное меню и экран правил.
 * 
 * @param window Ссылка на окно SFML для отрисовки.
 * @return true если пользователь начал игру, false если окно закрыто.
 */
bool showMenu(sf::RenderWindow &window) {
    sf::Font menuFont;
    if (!menuFont.loadFromFile("DejaVuSans-Bold.ttf")) {
        MessageBoxA(nullptr,"Failed to load font","Error",MB_ICONERROR);
        return false;
    }

    // Загружаем фон
    sf::Texture menuTexture;
    menuTexture.loadFromFile("bg.png");
    sf::Sprite menuBackground(menuTexture);
    menuBackground.setScale(
        static_cast<float>(window_width) / menuTexture.getSize().x,
        static_cast<float>(window_heighT) / menuTexture.getSize().y
    );

    // Заголовок
    sf::Text title("UGOLKI", menuFont, 72);
    title.setFillColor(sf::Color::Black);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    title.setPosition(window_width / 2.f, window_heighT / 7.f);

    // Кнопка Start
    sf::RectangleShape startButton(sf::Vector2f(250.f, 80.f));
    startButton.setFillColor(sf::Color(200, 200, 200));
    startButton.setPosition(window_width/2.f - 125.f, 300.f);

    sf::Text startButtonText("Start", menuFont, 36);
    startButtonText.setFillColor(sf::Color::Black);
    startButtonText.setPosition(window_width/2.f - 50.f, 315.f);

    // Кнопка Rules
    sf::RectangleShape rulesButton(sf::Vector2f(250.f, 80.f));
    rulesButton.setFillColor(sf::Color(200, 200, 200));
    rulesButton.setPosition(window_width/2.f - 125.f, 400.f);

    sf::Text rulesButtonText("Rules", menuFont, 36);
    rulesButtonText.setFillColor(sf::Color::Black);
    rulesButtonText.setPosition(window_width/2.f - 50.f, 415.f);

    // Кнопка Back (для экрана правил)
    sf::RectangleShape backButton(sf::Vector2f(250.f, 80.f));
    backButton.setFillColor(sf::Color(200, 200, 200));
    backButton.setPosition(window_width/2.f - 125.f, 1000.f);

    sf::Text backButtonText("Back", menuFont, 36);
    backButtonText.setFillColor(sf::Color::Black);
    backButtonText.setPosition(window_width/2.f - 50.f, 1015.f);

    // Текст правил
    sf::Text rulesText(
        "Rules of the game:\n\n"
		"1. Objective:\n"
		"   Move all your pieces from your starting corner to the opposite corner.\n\n"
		"2. Turn Order:\n"
		"   White (player) moves first, followed by Black (AI).\n\n"
		"3. Movement:\n"
		"   Pieces can move only horizontally or vertically, one cell at a time.\n"
		"   Diagonal moves are not allowed.\n\n"
		"4. Jumping:\n"
		"   You can jump over one adjacent piece (yours or opponent's) to an empty\n"
		"   cell directly behind it. Multiple consecutive jumps in a single move are allowed.\n\n"
		"5. End of Game:\n"
		"   The game ends when all pieces reach the opposite corners.\n"
		"   The player with more pieces in the destination corner wins.\n",
        menuFont, 36
    );
    rulesText.setFillColor(sf::Color::Black);
    rulesText.setPosition(100.f, 150.f);

    MenuState state = MenuState::MAIN;

    // цикл меню
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                float mx = static_cast<float>(event.mouseButton.x);
                float my = static_cast<float>(event.mouseButton.y);

                if (state == MenuState::MAIN) {
                    // Start
                    if (startButton.getGlobalBounds().contains(mx, my)) {
                        return true; // начинаем игру
                    }
                    // Rules
                    if (rulesButton.getGlobalBounds().contains(mx, my)) {
                        state = MenuState::RULES; // переключаемся на экран правил
                    }
                }
                else if (state == MenuState::RULES) {
                    // Back
                    if (backButton.getGlobalBounds().contains(mx, my)) {
                        state = MenuState::MAIN; // возвращаемся в меню
                    }
                }
            }
        }

        // Отрисовка
        window.clear();
        window.draw(menuBackground);

        if (state == MenuState::MAIN) {
            window.draw(title);
            window.draw(startButton);
            window.draw(startButtonText);
            window.draw(rulesButton);
            window.draw(rulesButtonText);
        }
        else if (state == MenuState::RULES) {
            window.draw(rulesText);
            window.draw(backButton);
            window.draw(backButtonText);
        }

        window.display();
    }

    return false;
}
// экран окончания игры
/**
 * @brief Показывает экран окончания игры.
 * 
 * @param window Ссылка на окно SFML.
 * @param resultText Текст результата (кто выиграл, счет).
 * @param font Шрифт для отображения текста.
 * 
 * Отображает затемненный фон, панель с результатом и закрывается через 5 секунд.
 */
void showGameOver(sf::RenderWindow &window, const std::string &resultText, sf::Font &font) {
    // затемняющий фон
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window_width), static_cast<float>(window_heighT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    // белая панель в центре
    sf::RectangleShape panel(sf::Vector2f(600.f, 300.f));
    panel.setFillColor(sf::Color(255, 255, 255, 230));
    panel.setOrigin(panel.getSize().x / 2.f, panel.getSize().y / 2.f);
    panel.setPosition(window_width / 2.f, window_heighT / 2.f);

    // текст результата (очки и победитель)
    sf::Text result(resultText, font, 36);
    result.setFillColor(sf::Color::Black);
    sf::FloatRect textBounds = result.getLocalBounds();
    result.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
    result.setPosition(window_width / 2.f, window_heighT / 2.f);

    // таймер
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
        }

        window.clear();
        window.draw(boardBackground);
        drawBoard(window, -1, -1, false);
        window.draw(overlay);
        window.draw(panel);
        window.draw(result);
        window.display();

        if (clock.getElapsedTime().asSeconds() > 5.f) {
            return; // закрываем экран Game Over через 3 сек
        }
    }
}
/**
 * @brief Сбрасывает игру в начальное состояние.
 * 
 * Выполняет повторную инициализацию доски, сброс истории ходов и состояния управления.
 */
void resetGame() {
    // расставляем фишки заново
    initBoard();

    // сбрасываем пометки попадания в чужой угол (в ai.h у тебя inOpponentCorner)
    for (int i = 0; i < board_size; ++i)
        for (int j = 0; j < board_size; ++j)
            inOpponentCorner[i][j] = false;

    moveHistory.clear(); // очищаем историю и счётчик ходов
    moveNumber = 0; // нумерация начнётся с 1 при первом успешном инкременте
    whiteMoves = blackMoves = 20;// сбрасываем лимиты ходов
    playerTurn = true;// выставляем начальные состояния управления
    selectedX = selectedY = -1;// сброс выбранной клетки/состояния выбора
    pieceSelected = false;
}
/**
 * @brief Точка входа в Windows-приложение.
 *
 * Аналог стандартной функции `main`, но в WinAPI. 
 * Используется как стартовая точка для GUI-программ под Windows.
 * 
 * Создает окно, отображает меню, запускает игровой цикл.
 * 
 * @param hInstance Дескриптор текущего экземпляра приложения. 
 *        Используется системой Windows для идентификации программы 
 *        и загрузки ресурсов (иконок, диалогов, строк и т.п.).
 * 
 * @param hPrevInstance Дескриптор предыдущего экземпляра приложения. 
 *        В современных версиях Windows всегда равен `NULL` 
 *        и не используется (оставлен для совместимости).
 * @param lpCmdLine Аргументы командной строки.
 * @param nCmdShow Флаг отображения окна.
 * @return Код возврата (0 — успешное завершение).
 */
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
    sf::RenderWindow window(sf::VideoMode(window_width,window_heighT),"UGOLKI");
    // показываем меню
    if (!showMenu(window)) {
        return 0; // если закрыли окно на меню
    }

	// Загружаем фон доски
	if (!boardBackgroundTexture.loadFromFile("bg.png")) {
		MessageBoxA(nullptr, "failed to load background", "Error", MB_ICONERROR);
	} else {
		boardBackground.setTexture(boardBackgroundTexture);
		boardBackground.setScale(
			static_cast<float>(window_width) / boardBackgroundTexture.getSize().x,
			static_cast<float>(window_heighT) / boardBackgroundTexture.getSize().y
		);
	}

    // инициализация доски и всех игровых переменных только после меню
    initBoard();

    if(!font.loadFromFile("DejaVuSans-Bold.ttf")){
        MessageBoxA(nullptr,"Failed to load font","Error",MB_ICONERROR);
        return -1;
    }

    // запускаем игровой цикл
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed) 
                window.close();

            if(playerTurn && whiteMoves>0 && event.type==sf::Event::MouseButtonPressed){
                int mouseX=event.mouseButton.x, mouseY=event.mouseButton.y;
                int gridX=(board_size-1)-((mouseY-border)/cell_size);
                int gridY=(mouseX-border)/cell_size;
                if(isInside(gridX,gridY)){
                    if(!pieceSelected && board[gridX][gridY]=='W'){
                        selectedX=gridX; selectedY=gridY; pieceSelected=true;
                    }else if(pieceSelected){
                        if(makeMove(selectedX,selectedY,gridX,gridY,'W')){
                            moveNumber++; whiteMoves--;
                            moveHistory.push_back(std::to_string(moveNumber)+". Player: "+
                                                  std::string(1,'A'+selectedY)+std::to_string(selectedX+1)+
                                                  " -> "+std::string(1,'A'+gridY)+std::to_string(gridX+1));
                            pieceSelected=false; 
                            playerTurn=false;
                        }else{
                            pieceSelected=false; 
                        }
                    }
                }
            }

            if(!playerTurn && blackMoves>0){
                if(!makeAIMove()) {
                    moveNumber++; 
                    moveHistory.push_back(std::to_string(moveNumber)+". AI: skipped");
                }
                playerTurn=true;
            }

            // прокрутка истории ходов
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (event.mouseWheelScroll.delta > 0) {
                        if (scrollOffset < (int)moveHistory.size() - max_visible_moves)
                            scrollOffset++;
                    } else if (event.mouseWheelScroll.delta < 0) {
                        if (scrollOffset > 0)
                            scrollOffset--;
                    }
                }
            }
        }

		window.clear();
		window.draw(boardBackground);
		drawBoard(window, selectedX, selectedY, pieceSelected);
		window.display();

        // конец игры (как у тебя уже есть)
		if (whiteMoves <= 0 && blackMoves <= 0) {
			int whiteCount = 0, blackCount = 0;
			for (int i = board_size - CORNER_SIZE; i < board_size; i++)
				for (int j = board_size - CORNER_SIZE; j < board_size; j++)
					if ((i + j) >= 2 * board_size - CORNER_SIZE - 1 && board[i][j] == 'W')
						whiteCount++;
			for (int i = 0; i < CORNER_SIZE; i++)
				for (int j = 0; j < CORNER_SIZE; j++)
					if ((i + j) <= CORNER_SIZE - 1 && board[i][j] == 'B')
						blackCount++;

			std::string result;
			if (whiteCount > blackCount) result = "WHITE wins (you)!";
			else if (blackCount > whiteCount) result = "BLACK wins (AI)!";
			else result = "Draw!";

			result += "\nWHITE: " + std::to_string(whiteCount) +
					  " BLACK: " + std::to_string(blackCount);

			showGameOver(window, result, font); // показать экран конца игры

			// возвращаемся в меню
			if (!showMenu(window)) return 0; 

			// начинаем новую игру после меню
			resetGame();
		}
    }
	
    return 0;
}