#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "player.h"
#include "map.h"
#include "utils.h"
#include "adventure.h"

#define DAY_HOURS 24


int safe_scan(int* value) { // Проверяю на введение числа, в противном случае возвращаю ошибку и обратно к введению значения.
	if (scanf("%d", value) != 1) {
		printf("Ошибка: Введено не число!\n");
		while (getchar() != '\n');
		return 0;
	}
	return 1;
}

void load_item_catalog(char catalog[MAX_ITEMS][MAX_NAME_LEN]) { // Считываю названия из файла в этот массив
	FILE* f_items = fopen("items.txt", "r");

	if (f_items != NULL) {
		int temp_id;
		char temp_name[MAX_NAME_LEN];
		while (fscanf(f_items, "%d %31s", &temp_id, temp_name) == 2) {
			if (temp_id >= 0 && temp_id < MAX_ITEMS) {
				strcpy(catalog[temp_id], temp_name);
			}
		}
		fclose(f_items);
	}
	else { // В противном случаем предупреждаю об отсутсвии шайтан файла и заполняю массив дефолтными нормисами
		printf("Предупреждение: Файл items.txt не найден! Загружены стандартные названия.\n");
		strcpy(catalog[ID_EMPTY], "Пусто");
		strcpy(catalog[ID_WOOD], "Древесина");
		strcpy(catalog[ID_STONE], "Камень");
		strcpy(catalog[ID_SEEDS], "Семена");
		strcpy(catalog[ID_HOE], "Тяпка");
		strcpy(catalog[ID_APPLE], "Яблоко");
		strcpy(catalog[ID_SHOVEL], "Лопата");
		strcpy(catalog[ID_IRON], "Железо");
		strcpy(catalog[ID_ROPE], "Веревка");
		strcpy(catalog[ID_CHAINSAW], "Бензопила");
	}
}

int find_item_id(const char catalog[MAX_ITEMS][MAX_NAME_LEN], const char* name) {
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (strcmp(catalog[i], name) == 0) {
			return i;
		}
	}
	return -1;
}

int save_to_diary(const char* name, int day, int hour, const int* inv, const char catalog[MAX_ITEMS][MAX_NAME_LEN]) {
	FILE* f_diary = fopen("diary.txt", "a");
	if (f_diary == NULL) {
		return 0;
	}

	fprintf(f_diary, "=== ЗАПИСЬ В ДНЕВНИКЕ ===\n");
	fprintf(f_diary, "Фермер: %s\n", name);
	fprintf(f_diary, "Время: %d день, %d час\n", day, hour);
	fprintf(f_diary, "Содержимое рюкзака:\n");
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		fprintf(f_diary, "  Слот %d: %s\n", i, catalog[inv[i]]);
	}
	fprintf(f_diary, "=========================\n\n");

	fclose(f_diary);
	return 1;
}


void free_map(char** map, int height) {
	if (map == NULL) {
		return;
	}
	for (int i = 0; i < height; i++) // для каждой строки в рамках условия освобождаю память каждой конкретной строки
		free(map[i]);
	}

	free(map); // освобождаю память массива указателей
}

char** allocate_map(int height, int width) {
	char** map = malloc(height * sizeof(char*)); // выделяю память под указатели

	if (map == NULL) {
		return NULL;
	}

	for (int i = 0; i < height; i++) { // записываю нули чтобы каждый указатель был изначально пустым
		map[i] = NULL;
	}

	for (int i = 0; i < height; i++) {
		map[i] = malloc(width * sizeof(char)); // выделяю память под сами строки

		if (map[i] == NULL) { // если не получилось выделить память у какой нибудь строке
			free_map(map, height);
			return NULL;
		}
	}

	return map;
}

void generate_map(char** map, int height, int width) {
	int obstacle_percent = MIN_OBSTACLE_PERCENT + rand() % (MAX_OBSTACLE_PERCENT - MIN_OBSTACLE_PERCENT + 1); // выбираю процент препятствий

	for (int i = 0; i < height; i++) { // перебираю строки
		for (int j = 0; j < width; j++) { // перебираю столбцы
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1) { 
				map[i][j] = '#'; // стенки
			}
			else {
				int chance = rand() % 100;

				if (chance < obstacle_percent) {
					map[i][j] = (rand() % 2 == 0) ? 'T' : 'R';  // препятствия
				}
				else {
					map[i][j] = '.'; // свободные для ходьбы поля
				}
			}
		}
	}
}

void print_map(char** map, int height, int width,
	int player_y, int player_x) {

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			if (i == player_y && j == player_x) { 
				printf("@ "); // игрок
			}
			else {
				printf("%c ", map[i][j]); // вывожу любой другой символ
			}
		}
		printf("\n");
	}
}

int control_player(char** map, int height, int width,
	int* player_y, int* player_x, int key) {

	int new_y = *player_y;
	int new_x = *player_x;

	switch (key) {
	case 'w':
	case 'W':
	case L'ц':
	case L'Ц':
		new_y--;
		break;

	case 's':
	case 'S':
	case L'ы':
	case L'Ы':
		new_y++;
		break;

	case 'a':
	case 'A':
	case L'ф':
	case L'Ф':
		new_x--;
		break;

	case 'd':
	case 'D':
	case L'в':
	case L'В':
		new_x++;
		break;

	default:
		return 0;
	}

	if (new_y < 0 || new_y >= height ||
		new_x < 0 || new_x >= width) {
		return 0;
	}

	if (map[new_y][new_x] == '#' ||
		map[new_y][new_x] == 'T' ||
		map[new_y][new_x] == 'R' ||
		map[new_y][new_x] == ' ') {
		return 0;
	}

	*player_y = new_y;
	*player_x = new_x;

	return 1;
}

int place_stairs(char** map, int height, int width, char stair,
	int excluded_y, int excluded_x) {

	int free_cells = 0;


	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (map[i][j] == '.' &&
				!(i == excluded_y && j == excluded_x)) {
				free_cells++;
			}
		}
	}

	if (free_cells == 0) {
		return 0;
	}

	int target = rand() % free_cells; // случайный номер клетки

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (map[i][j] == '.' &&
				!(i == excluded_y && j == excluded_x)) {

				if (target == 0) {
					map[i][j] = stair;
					return 1;
				}

				target--;
			}
		}
	}

	return 0;
}

	


int place_stairs_up(char** map, int height, int width,
	int* stair_y, int* stair_x) {

	int free_cells = 0;

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (map[i][j] == '.') {
				free_cells++;
			}
		}
	}

	if (free_cells == 0) {
		return 0;
	}

	int target = rand() % free_cells;

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (map[i][j] == '.') {

				if (target == 0) {
					map[i][j] = 'U';
					*stair_y = i;
					*stair_x = j;
					return 1;
				}

				target--;
			}
		}
	}

	return 0;
}


int possible_path(char** map, int height, int width,
	int start_y, int start_x, int target_y, int target_x) {

	int visited[30][30] = { 0 };

	int queue_y[900];
	int queue_x[900];

	int front = 0;
	int rear = 0;

	queue_y[rear] = start_y;
	queue_x[rear] = start_x;
	rear++;

	visited[start_y][start_x] = 1;

	int dy[4] = { -1, 1, 0, 0 };
	int dx[4] = { 0, 0, -1, 1 };

	while (front < rear) {

		int y = queue_y[front];
		int x = queue_x[front];
		front++;

		if (y == target_y && x == target_x) {
			return 1;
		}

		for (int i = 0; i < 4; i++) {

			int new_y = y + dy[i];
			int new_x = x + dx[i];

			if (new_y < 0 || new_y >= height ||
				new_x < 0 || new_x >= width) {
				continue;
			}

			if (visited[new_y][new_x]) {
				continue;
			}

			if (map[new_y][new_x] == '#' ||
				map[new_y][new_x] == 'T' ||
				map[new_y][new_x] == 'R') {
				continue;
			}

			visited[new_y][new_x] = 1;

			queue_y[rear] = new_y;
			queue_x[rear] = new_x;
			rear++;
		}
	}

	return 0;
}

int collapse_floor(char** map, int height, int width,
	int layer, int player_y, int player_x) {

	int top = 1 + layer;
	int bottom = height - 2 - layer;
	int left = 1 + layer;
	int right = width - 2 - layer;

	int stair_collapsed = 0;

	if (top > bottom || left > right) { // а существует ли слой? Если выполняется условие то уже нет
		return 0;
	}

	for (int i = top; i <= bottom; i++) { // перебираю клетки для подходящего слоя
		for (int j = left; j <= right; j++) {

			if (i != top && i != bottom && // не выбираем края а выбираем всю остальную доступную часть
				j != left && j != right) {
				continue;
			}

			if (i == player_y && j == player_x) { // защита клетки игрока
				continue;
			}

			if (map[i][j] == 'U') {
				stair_collapsed = 1; // а вот и обрушение
			}

			map[i][j] = ' ';
		}
	}

	return stair_collapsed;
}


void adventure(void) {
	printf("\n=== БАШНЯ ===\n");

	int depth = 1;
	int height;
	int width;
	int player_y = 1;
	int player_x = 1;
	int key;

	char** map = NULL;

	while (1) {

		height = MIN_MAP_SIZE +
			rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

		width = MIN_MAP_SIZE +
			rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

		map = allocate_map(height, width);

		if (map == NULL) {
			printf("Ошибка выделения памяти!\n");
			return;
		}

		int map_created = 0;
		int stair_y = -1;
		int stair_x = -1;

		for (int attempt = 0; attempt < 100; attempt++) {

			generate_map(map, height, width);

			player_y = 1;
			player_x = 1;

			map[player_y][player_x] = 'D';

			if (!place_stairs_up(map, height, width,
				&stair_y, &stair_x)) {
				continue;
			}

			if (possible_path(map, height, width,
				player_y, player_x, stair_y, stair_x)) {
				map_created = 1;
				break;
			}
		}

		if (!map_created) {
			printf("Не удалось создать проходимый этаж!\n");
			free_map(map, height);
			return;
		}

		printf("\nЭтаж: %d\n", depth);
		printf("Размер карты: %d x %d\n", height, width);

		print_map(map, height, width, player_y, player_x);

		int next_floor = 0;
		int moves = 0;
		int collapsed_layers = 0;
		int emergency_evacuation = 0;

		while (1) {

			if (emergency_evacuation) {
				printf("\nЛестница U провалилась! Срочно эвакуируйтесь: Q!\n");
			}
			else {
				printf("\nW A S D | Q - домой\n");
			}

			key = _getwch();

			if (key == 'q' || key == 'Q' ||
				key == L'й' || key == L'Й') {

				if (map[player_y][player_x] == 'D' ||
					emergency_evacuation) {

					free_map(map, height);
					printf("\nВы вернулись домой!\n");
					return;
				}

				printf("Чтобы вернуться домой, найдите D!\n");
				continue;
			}

			if (emergency_evacuation) {
				printf("Лестница вверх разрушена! Нажмите Q для эвакуации!\n");
				continue;
			}

			if (!control_player(map, height, width,
				&player_y, &player_x, key)) {
				printf("Нельзя пройти в этом направлении!\n");
				continue;
			}

			moves++;

			if (map[player_y][player_x] == 'U') {
				next_floor = 1;
				break;
			}

			if (moves % 5 == 0) {

				int stair_collapsed = collapse_floor(
					map, height, width,
					collapsed_layers,
					player_y, player_x
				);

				collapsed_layers++;

				if (stair_collapsed) {
					emergency_evacuation = 1;
				}
			}

			print_map(map, height, width, player_y, player_x);
		}

		if (next_floor) {
			free_map(map, height);
			map = NULL;
			depth++;
		}
	}
}


	

	
int main()
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	srand((unsigned int)time(NULL));
	int current_day = 1;
	int current_hour = 8;
	Player* player = NULL;
	int userAction;

	printf("Добро пожаловать!\n");
	player = create_player();

	if (player == NULL) {
		return 1;
	}

	char item_names[MAX_ITEMS][MAX_NAME_LEN];
	load_item_catalog(item_names, "items.txt");

	while (1) {
		printf("\n=== ПЕРВЫЕ ШАГИ ВЕСЁЛОГО ФЕРМЕРА. Здарова, %s ===\n", player->name);
		printf(" 0. Выйти из игры\n");
		printf(" 1. Посмотреть на часы\n");
		printf(" 2. Поработать на ферме\n");
		printf(" 3. Проверить инвентарь\n");
		printf(" 4. Положить предмет в инвентарь\n");
		printf(" 5. Выбросить предмет\n");
		printf(" 6. Проверить соседние слоты\n");
		printf(" 7. Поиск предмета в рюкзаке по названию\n");
		printf(" 8. Записать состояние в дневник фермера (diary.txt)\n");
		printf(" 9. Оценка рюкзака у скупщика\n");
		printf("10. Отправиться в приключение\n");
		printf("Выберите действие: ");

		if (!safe_scan("%d", &userAction)) { // Главное меню защищено от ввода букв
			continue;
		}
		switch (userAction) {
		case 0:
			printf("Выход из игры. Возвращайтесь ещё!\n");
			free_player(&player);
			return 0;

		case 1:
			printf("Текущее время : %d день %d час\n", current_day, current_hour);
			break;

		case 2: {
			int work_hours;
			printf("Сколько часов вы хотите потратить на работу? ");

			if (!safe_scan("%d", &work_hours)) break; // Безопасно считываю количество рабочих часов, при ошибке возвращаюсь обратно
			if (work_hours <= 0) {
				printf("Ошибка: Нельзя работать 0 или меньше часов. Введите заново число.\n");
				break;
			}

			current_hour += work_hours;

			if (current_hour >= DAY_HOURS) { // Если текущий час прошёл сквозь количество часов в день тоооо 
				current_day += current_hour / DAY_HOURS; // Добавляю к текущему дню результат деления текущего часа от часов в день
				current_hour %= DAY_HOURS; // Пишу текущее время через остаток от деления 
			}
			printf("Вы успешно поработали в течение %d ч.!\n", work_hours);
			break;
		}

		case 3:
			printf("\n--- СОДЕРЖИМОЕ ИНВЕНТАРЯ ---\n");
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				printf("Слот %d: [%d] - %s\n",
					i, player->inventory[i],
					item_names[player->inventory[i]]);
			}
			break;

		case 4: {
			int slot;
			int item_id;

			printf("Выберите слот инвентаря (0-%d): ", INVENTORY_SIZE - 1);
			if (!safe_scan("%d", &slot)) break;

			if (slot < 0 || slot >= INVENTORY_SIZE) { // Индекс слота не может быть ниже нуля или переходить дальше размера инвентаря
				printf("Ошибка: Неверный индекс слота!\n");
				break;
			}

			printf("Введите ID предмета (%d-%d): ", ID_EMPTY, ID_CHAINSAW);
			if (!safe_scan("%d", &item_id)) break;

			if (item_id < ID_EMPTY || item_id > ID_CHAINSAW) { // Нельзя ввести ID предмета ниже значения 0 и 9, упираемся на то что ID предметов расположены от 0 до 9
				printf("Ошибка: Такого предмета не существует!\n");
				break;
			}

			player->inventory[slot] = item_id;
			printf("Предмет с ID %d успешно положен в слот %d.\n", item_id, slot);
			break;
		}

		case 5: {
			int slot;
			printf("Какой предмет выбросить? (0-%d): ", INVENTORY_SIZE - 1);
			if (!safe_scan("%d", &slot)) break;

			if (slot < 0 || slot >= INVENTORY_SIZE) {
				printf("Ошибка: Неверный индекс слота!\n");
				break;
			}

			player->inventory[slot] = ID_EMPTY; // Записываю в ячейку значение ID_EMPTY, т.е. 0 и избавляемся так от предмета.
			printf("Предмет %d выброшен.\n", slot);
			break;
		}

		case 6: {
			int item_a;
			int item_b;
			int found = 0; // Переменная, нужная чтобы зафиксировать соседние слоты

			printf("Введите ID первого предмета. ");
			if (!safe_scan("%d", &item_a)) break;

			printf("Введите ID второго предмета. ");
			if (!safe_scan("%d", &item_b)) break;

			for (int i = 0; i < INVENTORY_SIZE - 1; i++) {
				if ((player->inventory[i] == item_a && player->inventory[i + 1] == item_b) || // Проверяем два варианта расположения вещей
					(player->inventory[i] == item_b && player->inventory[i + 1] == item_a)) {
					printf("Предметы лежат в слотах %d и %d.\n", i, i + 1);
					found = 1;
				}
			}
			if (!found) {
				printf("Эти предметы не лежат в соседних слотах инвентаря.\n");
			}
			break;
		}
		case 7: {
			char search_name[MAX_NAME_LEN];
			printf("Введите название предмета для поиска: ");
			if (!safe_scan("%31s", search_name)) break;
			while (getchar() != '\n');

			int target_id = find_item_id(search_name);

			if (target_id == -1) {
				printf("Предмета с названием \"%s\" нет в каталоге игры.\n", search_name);
				break;
			}

			int found_slots = 0;
			printf("Результаты поиска для \"%s\" (ID %d):\n", search_name, target_id);
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				if (player->inventory[i] == target_id) {
					printf("-> Найдено в слоте %d\n", i);
					found_slots++;
				}
			}

			if (found_slots == 0) {
				printf("Такого предмета сейчас нет в вашем рюкзаке.\n");
			}
			break;
		}

		case 8: {
			printf("\n--- СВОДКА ДЛЯ ДНЕВНИКА ---\n");
			printf("Фермер: %s\n", player->name);
			printf("Время: %d день, %d час\n", current_day, current_hour);
			printf("Содержимое рюкзака:\n");

			for (int i = 0; i < INVENTORY_SIZE; i++) {
				printf("  Слот %d: %s\n",
					i, item_names[player->inventory[i]]);
			}

			printf("---------------------------\n");
			save_to_diary("Состояние фермера сохранено.");
			printf("Запись добавлена в дневник!\n");

			break;
		}
		case 9: {
			if (value_inventory(player->inventory, item_names)) {
				printf("\nВедомость успешно сохранена в файл output.txt!\n");

				FILE* f_out_read = fopen("output.txt", "r");
				if (f_out_read != NULL) {
					char line[128];
					printf("\n--- СОДЕРЖИМОЕ ФАЙЛА OUTPUT.TXT ---\n");
					while (fgets(line, sizeof(line), f_out_read) != NULL) {
						printf("%s", line);
					}
					printf("-----------------------------------\n");
					fclose(f_out_read);
				}
			}
			else {
				printf("Не удалось создать файл output.txt!\n");
			}
			break;
		}

		case 10:
			adventure(player, &current_day, &current_hour);
			break;

		default:
			printf("Неверный пункт меню. Попробуйте еще раз.\n");
			break;
		}
	}

	return 0;
}
