#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#define cls system("cls")
#define pa system("pause")
#define shX snake[0][snakeS - 1]
#define shY snake[1][snakeS - 1]
#define sX snake[0]
#define sY snake[1]
using namespace std;
struct level_save {
	int record = 0;
	char name[32] = "Пусто";
};
enum {
	clBlack, clNavy, clGreen, clTeal, clBrown,
	clPurple, clOlive, clGray, clSilver, clBlue,
	clLime, clCyan, clRed, clMagneta, clYellow,
	clWhite
};
enum { K_UP = 72, K_LEFT = 75, K_RIGHT = 77, K_DOWN = 80, K_ENTER = 13, K_ESC = 27, K_SPACE = 32 };
struct {
	int x;
	int y;
}winSize = { 49, 28 };

void SetConsoleWinSizePosition(int width = 0, int heigth = 0, short x_position = 0, short y_position = 0) {
	HWND hWnd = GetForegroundWindow();
	HANDLE wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetWindowPos(hWnd, NULL, x_position, y_position, 0, 0, NULL);
	SMALL_RECT windowSize;
	windowSize.Bottom = heigth;
	windowSize.Left = 0;
	windowSize.Right = width;
	windowSize.Top = 0;
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	COORD bufferSize = { width + 1 , heigth + 1 };
	SetConsoleScreenBufferSize(wHnd, bufferSize);
}
void ConsoleColor(unsigned short Tex = clWhite, unsigned short Bg = clBlack) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	Bg = Bg << 4 | Tex;
	SetConsoleTextAttribute(hConsole, Bg);
}
void Cursor(short x_position, short y_position) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD _coordPosition = { x_position , y_position };
	SetConsoleCursorPosition(hConsole, _coordPosition);
}
void clean_str(int str, unsigned short Bg = clBlack) {
	ConsoleColor(clWhite, Bg);
	Cursor(0, str);
	for (int i = 0; i < winSize.x + 1; i++)
		cout << " ";
}
bool confirm() {
	ConsoleColor();
	Cursor((winSize.x / 2) - 5, (winSize.y / 2) - 3);
	cout << "Вы уверены?";
	int key;
	bool sel = false;
	while (1) {
		Cursor((winSize.x / 2) - 3, (winSize.y / 2) - 1);
		ConsoleColor(clWhite, sel == true ? clSilver : clBlack);
		cout << "Да";
		ConsoleColor();
		cout << " ";
		ConsoleColor(clWhite, sel == false ? clSilver : clBlack);
		cout << "Нет";
		ConsoleColor();
		cout << " ";
		key = _getch();
		if (key == K_LEFT || key == K_RIGHT)
			sel = !sel;
		if (key == K_ENTER)
			return sel;
		if (key == K_ESC)
			return 0;
	}
}
void newSave() {
	FILE* f = fopen("snake.sav", "wb");
	level_save tmp[12][5];
	if (f) {
		fwrite(&tmp, sizeof(level_save), 60, f);
		fclose(f);
	}
}
void SetSpace(int x, int y) {
	Cursor(x, y);
	cout << "  ";
}
bool find(int x, int y, int level[][62]) {
	for (int i = 0; i < 62; i++) {
		if (level[0][i] == x && level[1][i] == y)
			return 1;
	}
	return 0;
}
void rScore(int score) {
	ConsoleColor();
	Cursor(8, winSize.y - 1);
	cout << score;
}
bool find(int x, int y, int** arr, int size, int m = 0, int p = 0) {
	for (int i = p; i < size - m; i++) {
		if (arr[0][i] == x && arr[1][i] == y)
			return 1;
	}
	return 0;
}
void print_level(int level[][62], int type) {
	cls;
	if (type != -1) {
		/*for (int i = 0; i < winSize.y - 3; i++) {
			for (int j = 0; j < winSize.x; j += 2) {
				Color_(clWhite, find(j, i, level) ? clTeal : clBlack);
				Cursor(j, i);
				cout << "  ";
			}
		}  */
		ConsoleColor(clWhite, clTeal);
		for (int i = 0; i < 62; i++)
			SetSpace(level[0][i], level[1][i]);
	}
	clean_str(winSize.y - 3, clYellow);
	ConsoleColor();
	Cursor(1, winSize.y - 1);
	cout << "Очки - ";
}
void add_tail(int**& snake, int& snakeS) {
	snakeS++;
	int** tmp = new int* [2];
	tmp[0] = new int[snakeS];
	tmp[1] = new int[snakeS];
	for (int i = 1; i < snakeS - 1; i++) {
		tmp[0][i + 1] = sX[i];
		tmp[1][i + 1] = sY[i];
	}
	tmp[0][1] = sX[0];
	tmp[1][1] = sY[0];
	delete[] sX;
	delete[] sY;
	delete[] snake;
	snake = tmp;
}
void del_tail(int**& snake, int& snakeS, int type, int level[][62]) {
	snakeS--;
	int** tmp = new int* [2];
	tmp[0] = new int[snakeS];
	tmp[1] = new int[snakeS];
	for (int i = 0; i < snakeS; i++) {
		tmp[0][i] = sX[i + 1];
		tmp[1][i] = sY[i + 1];
	}
	ConsoleColor(clWhite, type != -1 && find(sX[1], sY[1], level) ? clTeal : clBlack);
	Cursor(sX[1], sY[1]);
	cout << "  ";
	delete[] sX;
	delete[] sY;
	delete[] snake;
	snake = tmp;
}
void del_snake(int**& snake) {
	delete[] sX;
	delete[] sY;
	delete[] snake;
}
void game(int type, int& score, int level[2][62] = 0, int round = 0) {
	int** snake = new int* [2],
		snakeS = 6,
		levelS = 62;
	sX = new int[snakeS];
	sY = new int[snakeS];
	for (int i = 0; i < snakeS; i++) {
		sX[i] = (i * 2) - (snakeS * 2) + 2;
		sY[i] = 11;
	}
	ConsoleColor(clGreen);
	Cursor(shX, shY);
	cout << "[]";
	char bArr[6] = { '&', '-', '+', '<', '>', 'D' };
	int appleX = shX,
		appleY = shY,
		bonusX,
		bonusY,
		dr = 2,
		speed = 200,
		key,
		tmr = 0,
		nextSpawn = 3 + (type == 0 ? (round - 1) * 20 : 0),
		bType;
	bool bltmp,
		inv = false,
		bActive = false;
	print_level(level, type);
	rScore(score);
	score--;
	while (true) {
		if ((find(shX, shY, snake, snakeS, 2, 1) && !inv) || (type != -1 && find(shX, shY, level) && !inv)) {
			del_snake(snake);
			return;
		}
		if (shX == appleX && shY == appleY) {
			inv = false;
			if (bActive) {
				bActive = false;
				Cursor(bonusX, bonusY);
				ConsoleColor();
				cout << "  ";
				tmr = 0;
				clean_str(winSize.y - 3, clYellow);
			}
			score++;
			rScore(score);
			if (speed > 50)
				speed -= 10;
			add_tail(snake, snakeS);
			do {
				appleY = rand() % (winSize.y - 4);
				appleX = rand() % winSize.x;
				if (appleX % 2 == 1)
					appleX--;
				bltmp = false;
				if (bonusX == appleX && bonusY == appleY)
					bltmp = true;
				if (find(appleX, appleY, snake, snakeS))
					bltmp = true;
				if (type != -1 && find(appleX, appleY, level))
					bltmp = true;
			} while (bltmp);
			ConsoleColor(clRed);
			Cursor(appleX, appleY);
			cout << "AP";
		}
		if (tmr > winSize.x) {
			bActive = false;
			tmr = 0;
			ConsoleColor();
			SetSpace(bonusX, bonusY);
		}
		if (bActive) {
			ConsoleColor(clWhite, clYellow);
			SetSpace(winSize.x - tmr, winSize.y - 3);
			tmr++;
		}
		if (score == nextSpawn) {
			clean_str(winSize.y - 3, clBlue);
			nextSpawn += 3;
			bActive = true;
			bType = rand() % 6;
			if (bType == 5 && type == -1)
				bType--;
			if (bType == 4 && speed <= 60)
				bType--;
			if (bType == 1 && snakeS <= 6)
				bType++;
			do {
				bonusY = rand() % (winSize.y - 4);
				bonusX = rand() % winSize.x;
				if (bonusX % 2 == 1)
					bonusX--;
				bltmp = false;
				if (bonusX == appleX && bonusY == appleY)
					bltmp = true;
				if (find(bonusX, bonusY, snake, snakeS))
					bltmp = true;
				if (type != -1 && find(bonusX, bonusY, level))
					bltmp = true;
			} while (bltmp);
			Cursor(bonusX, bonusY);
			ConsoleColor(clBlue);
			cout << bArr[bType] << bArr[bType];
		}
		if (bActive && bonusX == shX && bonusY == shY) {
			clean_str(winSize.y - 3, clYellow);
			bActive = false;
			tmr = 0;
			if (bType == 0) {
				score += (rand() % 2) + 1;
				rScore(score);
			}
			if (bType == 1)
				del_tail(snake, snakeS, type, level);
			if (bType == 2)
				add_tail(snake, snakeS);
			if (bType == 3)
				speed += 20;
			if (bType == 4)
				speed -= 20;
			if (bType == 5)
				inv = true;
		}
		for (int i = 0; i < 10 && type == 0; i++) {
			if (type == 0 && round == i + 1 && score >= 20 * (i + 1) && score < 200) {
				del_snake(snake);
				return;
			}
		}
		ConsoleColor(clLime, type != -1 && find(shX, shY, level) ? clTeal : clBlack);
		Cursor(shX, shY);
		cout << "[]";
		for (int i = 0; i < snakeS - 1; i++) {
			swap(sX[i], sX[i + 1]);
			swap(sY[i], sY[i + 1]);
		}
		ConsoleColor(clWhite, type != -1 && find(sX[0], sY[0], level) ? clTeal : clBlack);
		Cursor(sX[0], sY[0]);
		cout << "  ";
		shX = sX[snakeS - 2];
		shY = sY[snakeS - 2];
		if (_kbhit()) {
			key = _getch();
			if (key == 0xe0) {
				key = _getch();
				if (key == K_UP && dr != 1)
					dr = -1;
				if (key == K_LEFT && dr != 2)
					dr = -2;
				if (key == K_RIGHT && dr != -2)
					dr = 2;
				if (key == K_DOWN && dr != -1)
					dr = 1;
			}
			if (key == K_ESC) {
				cls;
				if (confirm())
					return;
				print_level(level, type);
				rScore(score);
				for (int i = 1; i < snakeS; i++) {
					ConsoleColor(i == snakeS - 1 ? clGreen : clLime);
					Cursor(sX[i], sY[i]);
					cout << "[]";
				}
				ConsoleColor(clRed);
				Cursor(appleX, appleY);
				cout << "AP";
				if (bActive) {
					Cursor(bonusX, bonusY);
					ConsoleColor(clBlue);
					cout << bArr[bType] << bArr[bType];
				}
			}
		}
		if (dr == 2 || dr == -2) {
			shX += dr;
			if (shX < 0)
				shX = winSize.x - 1;
			if (shX > winSize.x)
				shX = 0;
		}
		else {
			shY += dr;
			if (shY < 0)
				shY = winSize.y - 4;
			if (shY > winSize.y - 4)
				shY = 0;
		}
		ConsoleColor(clGreen, type != -1 && find(shX, shY, level) ? clTeal : clBlack);
		Cursor(shX, shY);
		cout << "[]";
		Sleep(speed);
	}
}
int start(int type) {
	int score = 0, levels[10][2][62] = {
		{ { 0,2,4,6,8,10,38,40,42,44,46,48,0,48,0,48,0,48,0,48,0,48,16,18,20,22,24,26,28,30,32,16,18,20,22,24,26,28,30,32,0,48,0,48,0,48,0,48,0,48,0,2,4,6,8,10,38,40,42,44,46,48 },{ 0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,3,3,4,4,5,5,9,9,9,9,9,9,9,9,9,15,15,15,15,15,15,15,15,15,19,19,20,20,21,21,22,22,23,23,24,24,24,24,24,24,24,24,24,24,24,24 } },
		{ { 0,12,14,16,0,2,4,6,8,10,18,20,22,24,26,28,30,38,40,42,44,46,48,0,0,0,14,48,48,48,34,48,48,48,14,48,0,0,0,0,2,4,6,8,10,18,20,22,24,26,28,30,38,40,42,44,46,48,32,34,36,48 },{ 0,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,7,8,9,9,10,11,12,12,13,14,15,15,16,17,18,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,24,24,24,24 } },
		{ { 0,10,48,8,6,40,44,4,28,2,30,40,44,0,18,32,16,34,36,14,36,12,38,10,40,28,42,24,26,22,24,26,22,24,6,20,8,38,10,36,12,34,12,14,32,16,30,48,6,18,46,4,6,8,20,44,6,42,40,0,38,48 },{ 0,0,0,1,2,2,2,3,3,4,4,4,4,5,5,5,6,6,6,7,7,8,8,9,9,10,10,11,11,12,12,12,13,13,14,14,15,15,16,16,17,17,18,18,18,19,19,19,20,20,20,21,21,21,21,21,22,22,23,24,24,24 } },
		{ { 30,30,30,30,30,0,2,4,6,8,10,12,14,16,18,22,24,26,28,30,32,34,36,40,42,44,46,48,18,18,18,18,18,0,2,4,6,8,12,14,16,18,20,22,24,26,30,32,34,36,38,40,42,44,46,48,18,18,18,18,18,18 },{ 0,1,2,3,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,7,8,9,10,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,19,20,21,22,23,24 } },
		{ { 16,34,48,14,32,46,12,30,44,10,28,42,8,26,40,6,24,38,4,22,2,20,48,0,18,46,16,44,14,42,40,38,8,36,6,34,4,2,48,0,28,46,26,44,10,24,42,8,22,40,6,20,38,4,18,36,2,16,34,0,14,32 },{ 0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,7,7,7,8,8,8,9,9,10,10,11,12,13,13,14,14,15,16,16,17,17,17,18,18,19,19,19,20,20,20,21,21,21,22,22,22,23,23,23,24,24,24 } } ,
		{ { 14,12,10,8,6,4,2,0,0,2,4,6,8,10,12,22,24,26,28,30,32,34,36,38,48,10,12,14,16,18,20,22,34,36,38,40,42,44,46,48,46,42,40,38,36,34,16,18,20,22,34,36,38,40,42,44,42,34,14,22,48,0 },{ 0,1,2,3,4,5,6,7,19,18,17,16,15,14,13,8,7,6,5,4,3,2,1,0,5,24,23,22,21,20,19,18,12,11,10,9,8,7,6,17,18,20,21,22,23,24,13,14,15,16,14,15,16,17,18,18,19,13,13,17,0,24 } },
		{ { 24,24,24,24,24,24,24,24,24,24,24,26,32,38,12,16,22,38,34,10,14,10,12,14,16,8,34,38,40,38,36,34,32,36,36,38,38,10,14,10,12,12,12,10,2,8,10,14,24,24,36,34,40,42,24,24,4,6,44,46,24,24 },{ 0,5,6,7,8,12,13,11,17,20,24,12,12,12,12,12,12,3,3,16,16,19,19,19,20,20,16,16,5,6,6,6,5,19,18,17,20,3,3,4,5,6,7,8,12,12,12,12,16,21,12,12,12,12,19,18,12,12,12,12,4,3 } },
		{ { 6,8,10,12,10,8,8,10,4,6,6,4,12,8,8,8,8,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,44,46,48,42,42,42,42,42,40,38,32,32,32,32,34,36,32,44,30,20,20,18,22,20,20,18,22,30,44 },{ 18,18,18,18,17,16,17,16,18,17,16,17,17,19,20,21,22,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,20,21,22,19,18,17,16,22,21,20,18,17,16,19,19,19,19,20,20,20,21,22,22,22,22,22 } },
		{ { 20,22,24,26,28,28,28,28,18,16,16,16,16,18,26,24,24,20,20,18,16,14,12,12,12,12,12,12,12,12,12,12,26,28,30,32,32,32,32,32,32,32,32,32,32,20,24,24,20,18,20,22,24,26,24,26,16,18,26,24,20,18 },{ 3,3,3,4,5,7,8,9,4,5,7,8,9,10,10,11,12,11,12,13,13,13,14,15,16,17,18,19,20,22,23,24,13,13,13,14,15,16,17,18,19,20,22,23,24,15,15,18,18,16,16,18,17,17,21,21,23,22,23,23,19,19 } },
		{ { 48,46,44,40,42,38,36,34,32,30,18,18,18,18,18,18,18,18,18,20,22,24,26,30,28,32,36,34,38,40,42,42,42,42,42,42,42,42,42,40,38,36,34,32,30,28,28,28,28,28,30,32,34,36,36,28,26,24,22,20,32,48 },{ 0,1,2,3,3,3,3,3,3,3,8,9,10,11,12,14,13,15,16,17,18,19,19,19,19,19,19,19,19,18,17,16,15,14,13,12,11,10,9,8,8,8,8,8,8,9,10,11,12,13,14,14,14,13,12,3,4,5,6,7,10,6 } }
	};
	if (type == -1)
		game(-1, score);
	if (type == 0) {
		int i = 0;
		do {
			i++;
			cls;
			ConsoleColor();
			Cursor((winSize.x / 2) - 5, (winSize.y / 2) - 6);
			cout << "Уровень " << i;
			Cursor((winSize.x / 2) - 11, (winSize.y / 2) - 4);
			cout << "Игра начнется через  ";
			for (int j = 0; j < 4; j++) {
				cout << "\b" << 3 - j;
				Sleep(1000);
			}
			game(0, score, levels[i - 1], i);
			if (score < 20 * i && i < 11)
				break;
		} while (i < 10);
	}
	if (type > 0)
		game(1, score, levels[type - 1]);
	ConsoleColor();
	cls;
	Cursor(3, 3);
	cout << "Игра окончена";
	Cursor(3, 5);
	cout << "Очки - " << score;
	Cursor(3, 5);
	level_save ltmp[12][5];
	FILE* f = fopen("snake.sav", "rb");
	if (!f)
		newSave();
	else
		fclose(f);
	f = fopen("snake.sav", "rb");
	if (!f) {
		Cursor(1, 10);
		cout << "Не удалось создать или открыть файл сохранений.";
		return 0;
	}
	fread(&ltmp, sizeof(level_save), 60, f);
	fclose(f);
	bool new_record = false;
	int sel = 1, key;
	if (score > ltmp[type + 1][4].record) {
		Cursor(3, 7);
		cout << "Вы установили новый рекорд!";
		new_record = true;
		sel = 0;
	}
	while (true) {
		for (int i = 0; i < 3; i++) {
			ConsoleColor((i == 0 && new_record) || i > 0 ? clWhite : clGray, sel == i ? clSilver : clBlack);
			Cursor(3, 9 + i);
			if (i == 0)
				cout << "Сохранить рекорд";
			if (i == 1)
				cout << "Начать заново";
			if (i == 2)
				cout << "Выйти в меню";
			ConsoleColor();
			cout << " ";
		}
		key = _getch();
		if (key == K_UP)
			sel = sel == 0 ? 2 : sel - 1;
		if (key == K_DOWN)
			sel = sel == 2 ? 0 : sel + 1;
		if (key == K_ENTER && sel == 1)
			return 1;
		if (key == K_ENTER && sel == 0 && new_record) {
			cls;
			int place = 4;
			for (int i = place - 1; i >= 0; i--) {
				if (score > ltmp[type + 1][i].record)
					place = i;
			}
			Cursor(1, 6);
			cout << "Введите ваше имя:";
			cin.getline(ltmp[type + 1][4].name, 32);
			ltmp[type + 1][4].record = score;
			for (int i = 4; i > place; i--)
				swap(ltmp[type + 1][i], ltmp[type + 1][i - 1]);
			cls;
			Cursor(3, 7);
			f = fopen("snake.sav", "wb");
			if (f) {
				fwrite(&ltmp, sizeof(level_save), 60, f);
				cout << "Рекорд успешно сохранен";
				new_record = false;
				fclose(f);
			}
			else
				cout << "Не удалось открыть файл сохранений.";
		}
		if (key == K_ESC || key == K_ENTER && sel == 2) {
			cls;
			if (!new_record || confirm())
				return 0;
			cls;
		}
	}
}
void Menu() {
	int sel = 0, type = 0, key;
	bool change_type = true;
	level_save ltmp[12][5];
	FILE* f = fopen("snake.sav", "rb");
	if (!f)
		newSave();
	else
		fclose(f);
	f = fopen("snake.sav", "rb");
	if (!f) {
		Cursor(1, 10);
		cout << "Не удалось создать или открыть файл сохранений.";
		return;
	}
	fread(&ltmp, sizeof(level_save), 60, f);
	fclose(f);
	while (true) {
		for (int i = 0; i < 3; i++) {
			ConsoleColor(clWhite, sel == i ? clSilver : clBlack);
			Cursor(3, 5 + i);
			if (i == 0)
				cout << "Начать игру";
			if (i == 1) {
				if (type == -1)
					cout << "< Без карты  >";
				if (type == 0)
					cout << "< По уровням >";
				if (type > 0) {
					cout << "< Уровень " << type;
					if (type < 10)
						cout << " ";
					cout << " >";
				}
			}
			if (i == 2)
				cout << "Выйти";
			ConsoleColor();
			cout << " ";
		}
		if (change_type) {
			Cursor(3, 9);
			cout << "Рекорды:";
			for (int i = 0; i < 5; i++) {
				clean_str(11 + i);
				Cursor(3, 11 + i);
				cout << i + 1 << ". " << ltmp[type + 1][i].record << " ";
				cout << ltmp[type + 1][i].name;
			}
			change_type = false;
		}
		key = _getch();
		if (key == K_RIGHT && sel == 1) {
			type = type == 10 ? -1 : type + 1;
			change_type = true;
		}
		if (key == K_LEFT && sel == 1) {
			type = type == -1 ? 10 : type - 1;
			change_type = true;
		}
		if (key == K_UP)
			sel = sel == 0 ? 2 : sel - 1;
		if (key == K_DOWN)
			sel = sel == 2 ? 0 : sel + 1;
		if (key == K_ENTER && sel == 0) {
			cls;
			while (start(type));
			f = fopen("snake.sav", "rb");
			fread(&ltmp, sizeof(level_save), 60, f);
			fclose(f);
			change_type = true;
			cls;
		}
		if (key == K_ESC || key == K_ENTER && sel == 2) {
			cls;
			if (confirm())
				return;
			change_type = true;
			cls;
		}
	}
}
void main() {
	srand((unsigned)time(NULL));
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	SetConsoleWinSizePosition(winSize.x, winSize.y);
	Menu();
	ConsoleColor();
	Cursor(0, winSize.y);
}