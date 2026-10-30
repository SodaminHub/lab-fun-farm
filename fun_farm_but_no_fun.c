#include <stdio.h>

#define INVENTORY_SIZE 10
#define DAY_HOURS 24

#define ID_EMPTY 0  // Задаю уникальные номера для предметов
#define ID_WOOD 1
#define ID_STONE 2
#define ID_SEEDS 3
#define ID_HOE 4
#define ID_APPLE 5
#define ID_SHOVEL 6
#define ID_IRON 7
#define ID_ROPE 8
#define ID_COIN 9

#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

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
		strcpy(catalog[ID_COIN], "Монета");
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

int main()
{
	int current_day = 1;
	int current_hour = 8;
	int userAction;
	int inventory[INVENTORY_SIZE] = { 6, 4, 0, 0, 3, 1, 1, 8, 2, 9 };
	
	char farmer_name[32] = "";
	printf("Добро пожаловать! Введите имя вашего фермера: ");
	if (scanf("%31s", farmer_name) != 1) {
		strcpy(farmer_name, "Безымянный");
	}
	while (getchar() != '\n');

	char item_names[MAX_ITEMS][MAX_NAME_LEN];
	load_item_catalog(item_names);
	
	while (1) {
		printf("\n=== ПЕРВЫЕ ШАГИ ВЕСЁЛОГО ФЕРМЕРА. Здарова, (%s) ===\n", farmer_name);
		printf(" 0. Выйти из игры\n");
		printf(" 1. Посмотреть на часы\n");
		printf(" 2. Поработать на ферме\n");
		printf(" 3. Проверить инвентарь\n");
		printf(" 4. Положить предмет в инвентарь\n");
		printf(" 5. Выбросить предмет\n");
		printf(" 6. Проверить соседние слоты\n");
		printf(" 7. Поиск предмета в рюкзаке по названию\n");
		printf(" 8. Записать состояние в дневник фермера (diary.txt)\n");
		printf("Выберите действие: ");

		if (!safe_scan(&userAction)) { // Главное меню защищено от ввода букв
			continue;
		}
		switch (userAction) {
		case 0:
			printf("Выход из игры. Возвращайтесь ещё!");
			return 0;

		case 1:
			printf("Текущее время : %d день %d час\n", current_day, current_hour);
			break;

		case 2: {
			int work_hours;
			printf("Сколько часов вы хотите потратить на работу? ");

			if (!safe_scan(&work_hours)) break; // Безопасно считываю количество рабочих часов, при ошибке возвращаюсь обратно
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
			for (int i = 0; i < INVENTORY_SIZE; i++) { // УБИЛ и УНИЧТОЖИЛ этот ваш СВИЧ и теперь оно просто подставляет название вещи из item_names по его ID
				printf("Слот %d: [%d] - %s\n", i, inventory[i], item_names[inventory[i]]);
			}
			break;

		case 4: {
			int slot;
			int item_id;

			printf("Выберите слот инвентаря (0-%d): ", INVENTORY_SIZE - 1);
			if (!safe_scan(&slot)) break;

			if (slot < 0 || slot >= INVENTORY_SIZE) { // Индекс слота не может быть ниже нуля или переходить дальше размера инвентаря
				printf("Ошибка: Неверный индекс слота!\n");
				break;
			}

			printf("Введите ID предмета (%d-%d): ", ID_EMPTY, ID_COIN);
			if (!safe_scan(&item_id)) break;

			if (item_id < ID_EMPTY || item_id > ID_COIN) { // Нельзя ввести ID предмета ниже значения 0 и 9, упираемся на то что ID предметов расположены от 0 до 9
				printf("Ошибка: Такого предмета не существует!\n");
				break;
			}

			inventory[slot] = item_id;
			printf("Предмет с ID %d успешно положен в слот %d.\n", item_id, slot);
			break;
		}

		case 5: {
			int slot;
			printf("Какой предмет выбросить? (0-%d): ", INVENTORY_SIZE - 1);
			if (!safe_scan(&slot)) break;

			if (slot < 0 || slot >= INVENTORY_SIZE) {
				printf("Ошибка: Неверный индекс слота!\n");
				break;
			}

			inventory[slot] = ID_EMPTY; // Записываю в ячейку значение ID_EMPTY, т.е. 0 и избавляемся так от предмета.
			printf("Предмет %d выброшен.\n", slot);
			break;
		}

		case 6: {
			int item_a;
			int item_b;
			int found = 0; // Переменная, нужная чтобы зафиксировать соседние слоты

			printf("Введите ID первого предмета. ");
			if (!safe_scan(&item_a)) break;

			printf("Введите ID второго предмета. ");
			if (!safe_scan(&item_b)) break;


			for (int i = 0; i < INVENTORY_SIZE - 1; i++) {

				if ((inventory[i] == item_a && inventory[i + 1] == item_b) || // Проверяем два варианта расположения вещей
					(inventory[i] == item_b && inventory[i + 1] == item_a)) {

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
			if (scanf("%31s", search_name) != 1) break;
			while (getchar() != '\n');

			int target_id = find_item_id(item_names, search_name);

			if (target_id == -1) {
				printf("Предмета с названием \"%s\" нет в каталоге игры.\n", search_name);
				break;
			}

			int found_slots = 0;
			printf("Результаты поиска для \"%s\" (ID %d):\n", search_name, target_id);
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				if (inventory[i] == target_id) {
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
			printf("Фермер: %s\n", farmer_name);
			printf("Время: %d день, %d час\n", current_day, current_hour);
			printf("Содержимое рюкзака:\n");
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				printf("  Слот %d: %s\n", i, item_names[inventory[i]]);
			}
			printf("---------------------------\n");

			if (save_to_diary(farmer_name, current_day, current_hour, inventory, item_names)) {
				printf("Запись успешно добавлена в дневник (diary.txt)!\n");
			}
			else {
				printf("Ошибка: Не удалось открыть или записать в дневник!\n");
			}
			break;
		}

		default:
			printf("Неверный пункт меню. Попробуйте еще раз.\n");
			break;
		}
	}

    return 0;
}