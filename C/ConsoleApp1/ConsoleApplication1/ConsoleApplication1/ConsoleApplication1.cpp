#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<conio.h>


 #define FIELD_HEIGHT 64
 #define FIELD_WIDTH 64

 #define AREA_MAX 64
 #define AREA_SIZE_MIN 16

 #define SCREEN_RANGE 8

enum {
	CELL_TYPE_NONE,
	CELL_TYPE_WALL,
	CELL_TYPE_STAIRS,
	CELL_TYPE_AMULET,
	CELL_TYPE_PLAYER,
	CELL_TYPE_ENEMY
};


char cellAA[][2 + 1] = {
	". ",// CELL_TYPE_NONE
	"■",// CELL_TYPE_WALL
	"％",// CELL_TYPE_STAIRS
	"・",// CELL_TYPE_AMULET
	"＠",// CELL_TYPE_PLAYER
	"＆",// CELL_TYPE_ENEMY
};

typedef struct {
	int x, y, w, h;
}ROOM;

typedef struct {
	int x, y, w, h;
	ROOM room;
}AREA;

AREA areas[AREA_MAX];

typedef struct {
	int x, y;
	int hp, maxHp;
	int attack;
}CHARACTER;

CHARACTER player;
CHARACTER enemy;

int areaCount;
int floor = 1;

int field[FIELD_HEIGHT][FIELD_WIDTH];

void displayArea() {
	int buffer[FIELD_HEIGHT][FIELD_WIDTH] = {};
	for (int i = 0;i < areaCount;i++)
		for (int y = areas[i].y;y < areas[i].y + areas[i].h;y++)
			for (int x = areas[i].x;x < areas[i].x + areas[i].w;x++)
				buffer[y][x] = i;
	system("cls");
	for (int y = 0;y < FIELD_HEIGHT;y++) {
		for (int x = 0;x < FIELD_WIDTH;x++) {
			char str[] = "０";
			str[1] += buffer[y][x] % 10;
			printf("%s", str);
		}
		printf("\n");
	}
}

void spritArea(int _areaIdx) {
	int newAreaIdx = areaCount;
	int w = areas[_areaIdx].w;
	int h = areas[_areaIdx].h;

	if (rand() % 2) {
		areas[_areaIdx].w /= 2;

		areas[newAreaIdx].x = areas[_areaIdx].x + areas[_areaIdx].w;
		areas[newAreaIdx].y = areas[_areaIdx].y;
		areas[newAreaIdx].w = w - areas[_areaIdx].w;
		areas[newAreaIdx].h = areas[_areaIdx].h;
	}
	else {
		areas[_areaIdx].h /= 2;

		areas[newAreaIdx].x = areas[_areaIdx].x;
		areas[newAreaIdx].y = areas[_areaIdx].y + areas[_areaIdx].h;
		areas[newAreaIdx].w = areas[_areaIdx].w;
		areas[newAreaIdx].h = h - areas[_areaIdx].h;
	}

	if ((areas[_areaIdx].w < AREA_SIZE_MIN) || (areas[_areaIdx].h < AREA_SIZE_MIN) || (areas[newAreaIdx].w < AREA_SIZE_MIN) || (areas[newAreaIdx].h < AREA_SIZE_MIN)){
		areas[_areaIdx].w = w;
		areas[_areaIdx].h = h;
		return;
	}
	areaCount++;

	spritArea(_areaIdx);
	spritArea(newAreaIdx);
}

void setRandPosition(int*_pX, int*_pY) {
	int areaIdx = rand() % areaCount;
	*_pX = areas[areaIdx].room.x + rand() % areas[areaIdx].room.w;
	*_pY = areas[areaIdx].room.y + rand() % areas[areaIdx].room.h;
}

void generateField() {
	areaCount = 0;
	areas[0].x = 0;//0は独断
	areas[0].y = 0;
	areas[0].w = FIELD_WIDTH;
	areas[0].h = FIELD_HEIGHT;
	areaCount++;
	spritArea(0);

	for (int y = 0;y < FIELD_HEIGHT;y++)
		for (int x = 0;x < FIELD_WIDTH;x++)
			field[y][x] = CELL_TYPE_WALL;

	for (int i = 0;i < areaCount;i++) {
		areas[i].room.x = areas[i].x + 2;
		areas[i].room.y = areas[i].y + 2;
		areas[i].room.w = areas[i].w - 4;
		areas[i].room.h = areas[i].h - 4;

		for (int y = areas[i].room.y;y < areas[i].room.y + areas[i].room.h;y++)
			for (int x = areas[i].room.x;x < areas[i].room.x + areas[i].room.w;x++)
				field[y][x] = CELL_TYPE_NONE;

		for (int x = areas[i].x;x < areas[i].x + areas[i].w;x++)
			field[areas[i].y + areas[i].h - 1][x] = CELL_TYPE_NONE;

		for (int y = areas[i].y;y < areas[i].y + areas[i].h;y++)
			field[y][areas[i].x + areas[i].w - 1] = CELL_TYPE_NONE;

		for (int y2 = areas[i].y;y2 < areas[i].room.y;y2++)
			field[y2][areas[i].x + areas[i].room.w / 2] = CELL_TYPE_NONE;

		for (int y2 = areas[i].room.y + areas[i].room.h;y2 < areas[i].y + areas[i].h;y2++)
			field[y2][areas[i].x + areas[i].room.w / 2] = CELL_TYPE_NONE;

		for (int x2 = areas[i].x;x2 < areas[i].room.x;x2++)
			field[areas[i].y + areas[i].room.h / 2][x2] = CELL_TYPE_NONE;

		for (int x2 = areas[i].room.x + areas[i].room.w;x2 < areas[i].x + areas[i].w;x2++)
			field[areas[i].y + areas[i].room.h / 2][x2] = CELL_TYPE_NONE;
	}

	for (int y = 0;y < FIELD_HEIGHT;y++)
		field[y][FIELD_WIDTH - 1] = CELL_TYPE_WALL;

	for (int x = 0;x < FIELD_WIDTH;x++)
		field[FIELD_HEIGHT - 1][x] = CELL_TYPE_WALL;

	while(1) {
		bool filled = false;
		for (int y = 0;y < FIELD_HEIGHT;y++)
			for (int x = 0;x < FIELD_WIDTH;x++) {
				if (field[y][x] == CELL_TYPE_WALL)
					continue;

				int v[][2] = {
					{0,-1},
					{-1,0},
					{0,1},
					{1,0},
				};
				int n = 0;
				for (int i = 0;i < 4;i++) {
					int x2 = x + v[i][0];
					int y2 = y + v[i][1];
					if ((x2 < 0) || (x2 >= FIELD_WIDTH) || (y2 < 0) || (y2 >= FIELD_HEIGHT))
						n++;
					else if (field[y2][x2] == CELL_TYPE_WALL)
						n++;
				}
				if (n >= 3) {
					field[y][x] = CELL_TYPE_WALL;
					filled = true;
				}
			}
		if (!filled)
			break;
	}

	setRandPosition(&player.x,&player.y);
	{
		int x, y;
		setRandPosition(&x, &y);
		field[y][x] = (floor < 3) ? CELL_TYPE_STAIRS : CELL_TYPE_AMULET;
	}

	setRandPosition(&enemy.x, &enemy.y);
	enemy.hp = enemy.maxHp = 3 + floor*2;
	enemy.attack = 2 + floor;
}

void drawField() {
	system("cls");
	for (int y = 0;y < FIELD_HEIGHT;y++)
		for (int x = 0;x < FIELD_WIDTH;x++)
/*			if ((x == player.x) && (y == player.y))
				printf("＠");
			else*/
				printf("%s", cellAA[field[y][x]]);
}

int getRoom(int _x,int _y) {
	for(int i=0;i<areaCount;i++)
		if ((_x >= areas[i].room.x) && (_x < areas[i].room.x + areas[i].room.w)
			&& (_y >= areas[i].room.y) && (_y < areas[i].room.y + areas[i].room.h)) {
			return i;
		}
	return -1;
}

void display() {
	int buffer[FIELD_HEIGHT][FIELD_WIDTH];
	memcpy(buffer, field, sizeof field);

	if (player.hp > 0)
		buffer[player.y][player.x] = CELL_TYPE_PLAYER;
	if (enemy.hp > 0)
		buffer[enemy.y][enemy.x] = CELL_TYPE_ENEMY;

	system("cls");
	printf("%dF HP:%d/%d\n", floor, player.hp, player.maxHp);
	for (int y = player.y - SCREEN_RANGE;y < player.y + SCREEN_RANGE;y++) {
		for (int x = player.x - SCREEN_RANGE;x < player.x + SCREEN_RANGE;x++) {
			if ((x < 0) || (x >= FIELD_WIDTH) || (y < 0) || (y >= FIELD_HEIGHT))
				printf("■");
			else
				printf("%s", cellAA[buffer[y][x]]);
		}
		printf("\n");
	}
}

int main() {
	int turn = 0;
	srand((unsigned int)time(NULL));

	player.hp = player.maxHp = 15 + floor;
	player.attack = 2 + floor;

	generateField();

	while (1) {
		display();

		int x = player.x;
		int y = player.y;
		int int_key = _getch();
		switch ((int_key == 0x00 || int_key == 0xe0) ? _getch() : int_key) {
		case 0x1b:break;// ESC
		case 0x08:break;// BackSpace
		case 0x09:break;// TAB
		case 0x03:break;// CTRL-C
		case 0x0d:break;// ENTER
		case 0x20:break;// Space
		case 0x53:break;// DEL
		case 0x3b:break;// F1
		case 0x3c:break;// F2
		case 0x3d:break;// F3
		case 0x3e:break;// F4
		case 0x48:y--;    break;// 上
		case 0x4b:x--;    break;// 左
		case 0x50:y++;    break;// 右
		case 0x4d:x++;    break;// 下
		case'w':y--;    break;//移動用
		case's':y++;    break;//移動用
		case'a':x--;    break;//移動用
		case'd':x++;    break;//移動用
		default:break;//それ以外
		}

		if ((enemy.hp > 0) && (x == enemy.x) && (y == enemy.y)) {
			printf("プレイヤーの攻撃\n");
			_getch();
			int attack = player.attack / 2 + rand() % player.attack;
			enemy.hp -= attack;
			printf("敵(Lv.%d)に%dのダメージ\n", floor, attack);
			_getch();
			if (enemy.hp <= 0) {
				display();
				printf("敵(Lv.%d)を倒した\n", floor);
				_getch();
			}
		}
		else {
			switch (field[y][x]) {
			case CELL_TYPE_NONE:
				player.x = x;
				player.y = y;
				if ((turn % 25 == 0) && (player.hp < player.maxHp))
					player.hp++;
				break;

			case CELL_TYPE_WALL:
				break;

			case CELL_TYPE_STAIRS:
				floor++;
				generateField();
				break;

			case CELL_TYPE_AMULET:
				printf("Complete\n");
				printf("\a");
				_getch();
				return 0;
				break;
			}
		}

		if (enemy.hp > 0) {
			int room = getRoom(enemy.x, enemy.y);
			int distance = abs(player.x - enemy.x) + abs(player.y - enemy.y);
			if (distance == 1) {
				printf("敵(Lv.%d)の攻撃\n", floor);
				_getch();
				int attack = enemy.attack / 2 + rand() % enemy.attack;
				player.hp -= attack;
				printf("プレイヤーは%dダメージを受けた\n", attack);
				_getch();

				if (player.hp < 0) {
					display();
					printf("プレイヤーは意識が遠くなった\n");
					printf("GAME OVER\a");
					_getch();
					return 0;
				}
			}
			else if (
				((room > 0) && (room == getRoom(player.x, player.y)))
				|| (distance == 2)
				) {
				int x = enemy.x;
				int y = enemy.y;

				if (x < player.x)
					x++;
				else if (x > player.x)
					x--;
				else if (y < player.y)
					y++;
				else if (y > player.y)
					y--;


				if (field[y][x] != CELL_TYPE_WALL) {
					enemy.x = x;
					enemy.y = y;
				}
			}
		}
		turn++;
	}
}
