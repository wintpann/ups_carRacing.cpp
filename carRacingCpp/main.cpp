#include <iostream>
#include <conio.h>
#include <ctime>
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;

int g_counter;
const int g_lanes = 5;				// modify lanes amount here (max-5)
const int g_height = 20;				// modify height here
const int g_enemiesAmount = g_lanes * g_height / 8;	// modify enemies amount here 
const int g_width = g_lanes * 7 + 1;				// not here
int g_jumpTime = 15;					// modify injump time
int g_jumpAddFrequency = 100;			// modify frequency of jump's adding
int g_enemyAddFrequency = 1000 / (g_lanes * g_lanes / 3 * g_height);	// modify frequency of enemies' adding
int g_speedChanging = 100;
char g_map[g_height][g_width];	
int g_inJumpCounter = 0;				// counter for exit from jump
int g_index = 0;						// current enemy index
bool g_dyagnosticData = false;

enum Direction {
	STOP,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	JUMP,
	DD
};
struct CarXY {
	int carX, carY;
};
struct CarData {
	CarXY XY;
	//CarXY carDots[12];
	Direction dir;
};
struct GameData {
	bool gameOver;
	int speed;
	int score;
	int jumps;
	bool InJump;
	CarData myCar;
	vector<CarData> enemies;
};
void Setup(GameData &data) {
	HWND hwnd;
	hwnd = GetConsoleWindow();
	MoveWindow(hwnd, 100, 100, 600, 600, TRUE);
	SetConsoleTitle("Car Racing v 1.0");
	srand(static_cast<unsigned int>(time(0)));
	rand();
	g_counter = 1; // if it starts from 0, you have +1 jump and +1 enemy in the begining
	g_enemyAddFrequency = 1000 / (g_lanes * g_lanes / 3 * g_height);
	data.gameOver = false;
	data.speed = 5;
	data.score = 0;
	data.jumps = 1;
	data.InJump = false;
	data.myCar.XY.carX = 9;
	data.myCar.XY.carY = 10;
	data.myCar.dir = STOP;
	data.enemies.resize(g_enemiesAmount);
	for (int i = 0; i < g_enemiesAmount; i++) {
		data.enemies[i].XY.carX = 100;
		data.enemies[i].XY.carY = 100;
	}
}
void DyagnosticData() {
	cout << "\n";
	cout << "counter: " << g_counter << "\n";
	cout << "enemies amount: " << g_enemiesAmount << "\n";
	cout << "enemy adding frequency: " << g_enemyAddFrequency << "\n";
	cout << "enemy index: " << g_index;
}
void Input(GameData &data) {
	Direction &dir = data.myCar.dir;
	dir = STOP;
	if (_kbhit()) {
		switch (_getch()) {
		case 'a':
		case'A':
		case'4':
			dir = LEFT;
			break;
		case 'd':
		case 'D':
		case'6':
			dir = RIGHT;
			break;
		case 'w':
		case 'W':
		case'8':
			dir = UP;
			break;
		case 's':
		case'S':
		case'2':
			dir = DOWN;
			break;
		case ' ':
			if (data.jumps > 0 && data.InJump == false) {
				dir = JUMP;
				data.jumps--;
			}
			break;
		case '	':
			dir = DD;
			break;
		}
		
	}
}

void Logic(GameData &data) {
	Direction &dir = data.myCar.dir;		// moving my car - begin
	int &carX = data.myCar.XY.carX;
	int &carY = data.myCar.XY.carY;
	switch (dir) {
	case STOP:
		break;
	case LEFT:
		if (carX != 2)
			carX -= 7;
		break;
	case RIGHT:
		if (carX != g_width - 6)
			carX += 7;
		break;
	case UP:
		if (carY != 1)
			carY -= 2;
		break;
	case DOWN:
		if (carY != g_height-4)
			carY += 2;
		break;
	case JUMP:
		data.InJump = true;
		g_inJumpCounter = g_counter;
		break;
	case DD:
		g_dyagnosticData = !g_dyagnosticData;
		break;
	}										// moving my car - end

	g_counter++;
	data.score = g_counter / 10;

	if (g_counter % g_jumpAddFrequency == 1)				// adding 1 jump
		data.jumps++;

	if (g_counter - g_inJumpCounter == g_jumpTime) {	// stopping current jump
		data.InJump = false;
		g_inJumpCounter = 0;
	}

	if (g_counter % g_enemyAddFrequency == 1) {			// adding next enemy to lane
		int lane = rand() % g_lanes;
		data.enemies[g_index].XY.carX = lane * 7 + 2;
		data.enemies[g_index].XY.carY = 0;
		g_index++;
		if (g_index >= g_enemiesAmount)
			g_index = 0;
	}

	

	for (int i = 0; i < g_enemiesAmount; i++)			// moving all enemies
		data.enemies[i].XY.carY++;

	if (!data.InJump) {
		int &carX = data.myCar.XY.carX;
		int &carY = data.myCar.XY.carY;
		for (int i = 0; i < g_enemiesAmount; i++) {
			int &enemyX = data.enemies[i].XY.carX;
			int &enemyY = data.enemies[i].XY.carY;
			if (carX == enemyX) {
				int distance = enemyY - carY;
				if (distance>=-2&&distance<=2) {
					data.gameOver = true;
					break;
				}
			}
		}
	}
	
	if (g_counter%g_speedChanging == 1 && g_enemyAddFrequency!=2)
		g_enemyAddFrequency--;

	Sleep(data.speed);

}
void BuildMap(GameData &data) {
	for (int i = 0; i < g_height; i++) {				// filling empty cells
		for (int j = 0; j < g_width; j++) {
			g_map[i][j] = ' ';
		}
	}
	for (int i = 0; i < g_height; i++) {				// filling g_map
		for (int j = 0; j < g_width; j++) {
			
			
			for (int k = 0; k < g_enemiesAmount; k++) {						// enemies' cars
				if (i == data.enemies[k].XY.carY&&j == data.enemies[k].XY.carX) {
					g_map[i][j] = '*';
					g_map[i][j + 1] = '`';
					g_map[i][j + 2] = '`';
					g_map[i][j + 3] = '*';
					g_map[i + 1][j] = '|';
					g_map[i + 1][j + 3] = '|';
					g_map[i + 2][j] = '*';
					g_map[i + 2][j + 1] = '_';
					g_map[i + 2][j + 2] = '_';
					g_map[i + 2][j + 3] = '*';

				}
			}

			if (i == data.myCar.XY.carY&&j == data.myCar.XY.carX) {		// my car
				if (!data.InJump&&!data.gameOver) {						// normally 
					g_map[i][j] = '*';
					g_map[i][j + 1] = '^';
					g_map[i][j + 2] = '^';
					g_map[i][j + 3] = '*';
					g_map[i + 1][j] = '|';
					g_map[i + 1][j + 3] = '|';
					g_map[i + 2][j] = '*';
					g_map[i + 2][j + 1] = '-';
					g_map[i + 2][j + 2] = '-';
					g_map[i + 2][j + 3] = '*';
				}
				else if (data.InJump && !data.gameOver) {				// in jump
					g_map[i][j] = '^';
					g_map[i][j + 1] = '^';
					g_map[i][j + 2] = '^';
					g_map[i][j + 3] = '^';
					g_map[i + 1][j] = '^';
					g_map[i + 1][j + 3] = '^';
					g_map[i + 2][j] = '^';
					g_map[i + 2][j + 1] = '^';
					g_map[i + 2][j + 2] = '^';
					g_map[i + 2][j + 3] = '^';
				}
				else {													// collision
					g_map[i][j] = '*';
					g_map[i][j + 1] = '*';
					g_map[i][j + 2] = '*';
					g_map[i][j + 3] = '*';
					g_map[i+1][j] = '*';
					g_map[i+1][j + 1] = '*';
					g_map[i+1][j + 2] = '*';
					g_map[i+1][j + 3] = '*';
					g_map[i+2][j] = '*';
					g_map[i+2][j + 1] = '*';
					g_map[i+2][j + 2] = '*';
					g_map[i+2][j + 3] = '*';
				}
				
			}


			if (i == 0 && j != g_width - 1 && j != 0)	// top horizontal wall
				g_map[i][j] = '_';

			if (i != 0 && (j == 0 || j == g_width - 1))	// vertical walls
				g_map[i][j] = '|';

			if (i == g_height - 1 && j != 0 && j != g_width - 1)	// bottom horizontal wall
				g_map[i][j] = '_';


			for (int k = 7; k < g_width - 7; k += 7) {
				if (j == k && i != 0)	// intermediates walls
					g_map[i][j] = ':';
			}
		}
	}
}
void DrawMap(const GameData &data) {
	string strOut;
	for (int i = 0; i < g_height; i++) {
		strOut = "";
		for (int j = 0; j < g_width; j++) {
			strOut += g_map[i][j];
		}
		cout << strOut << "\n";
	}
	cout << "Score: " << data.score << "\nJumps available: " << data.jumps << "\n";
	if (g_dyagnosticData)
		DyagnosticData();
}
int main() {
	GameData data;
	while (true) {
		Setup(data);
		do {
			Input(data);
			Logic(data);
			BuildMap(data);
			system("cls");
			DrawMap(data);
		} while (!data.gameOver);
		cout << "\nGAME OVER\nPress any key to start over";
		_getch();
	}
	return 0;
}
