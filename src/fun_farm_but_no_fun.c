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

	char item_names[MAX_ITEMS][MAX_NAME_LEN];
	load_item_catalog(item_names, "items.txt");

	while (1) {
		if (player != NULL) {
			printf("\n=== ПЕРВЫЕ ШАГИ ВЕСЁЛОГО ФЕРМЕРА. Здарова, %s ===\n", player->name);
		}
		else {
			printf("\n=== ПЕРВЫЕ ШАГИ ВЕСЁЛОГО ФЕРМЕРА ===\n");
		}
		printf(" 0. Выйти из игры\n");
		printf(" 1. Создать / пересоздать персонажа\n");
		printf(" 2. Карточка персонажа\n");
		printf(" 3. Посмотреть на часы\n");
		printf(" 4. Поработать на ферме\n");
		printf(" 5. Проверить инвентарь\n");
		printf(" 6. Положить предмет в инвентарь\n");
		printf(" 7. Выбросить предмет\n");
		printf(" 8. Проверить соседние слоты\n");
		printf(" 9. Поиск предмета в рюкзаке по названию\n");
		printf("10. Записать состояние в дневник фермера (diary.txt)\n");
		printf("11. Оценка рюкзака у скупщика\n");
		printf("12. Отправиться в приключение\n");

		if (!safe_scan("%d", &userAction)) { // Главное меню защищено от ввода букв
			continue;
		}
		if (player == NULL && userAction >= 3 && userAction <= 12) {
			printf("Сначала создайте персонажа!\n");
			continue;
		}

		switch (userAction) {
		case 0:
			printf("Выход из игры. Возвращайтесь ещё!\n");
			free_player(&player);
			return 0;

		case 1: {
			if (player != NULL) {
				int answer;

				printf("Персонаж уже существует. Вы уверены, что хотите удалить его и создать нового?\n");
				printf("1 — Да, пересоздать\n");
				printf("0 — Нет, вернуться в меню\n");

				if (!safe_scan("%d", &answer)) {
					break;
				}

				if (answer != 1) {
					printf("Пересоздание отменено.\n");
					break;
				}

				free_player(&player);
			}

			player = create_player();

			if (player == NULL) {
				printf("Не удалось создать персонажа.\n");
			}

			break;
		}

		case 2:
			if (player == NULL) {
				printf("Сначала создайте персонажа (пункт 1)!\n");
				break;
			}

			show_player(player);

			printf("Дата: %d день\n", current_day);
			printf("Время: %02d:00\n", current_hour);

			printf("Инвентарь:\n");
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				int item_id = player->inventory[i];

				if (item_id >= 0 && item_id < MAX_ITEMS) {
					printf("  Слот %d: %s\n", i, item_names[item_id]);
				}
				else {
					printf("  Слот %d: Неизвестный предмет\n", i);
				}
			}
			break;

		case 3:
			printf("Текущее время : %d день %d час\n", current_day, current_hour);
			break;

		case 4: {
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

		case 5:
			printf("\n--- СОДЕРЖИМОЕ ИНВЕНТАРЯ ---\n");
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				int item_id = player->inventory[i];

				if (item_id >= 0 && item_id < MAX_ITEMS) {
					printf("Слот %d: [%d] - %s\n",
						i, item_id, item_names[item_id]);
				}
				else {
					printf("Слот %d: Некорректный ID предмета\n", i);
				}
			}
			break;

		case 6: {
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

		case 7: {
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

		case 8: {
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
		case 9: {
			char search_name[MAX_NAME_LEN];
			printf("Введите название предмета для поиска: ");
			if (!safe_scan("%31s", search_name)) break;
			while (getchar() != '\n');

			int target_id = find_item_id(item_names, search_name);

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

		case 10: {
			printf("\n--- СВОДКА ДЛЯ ДНЕВНИКА ---\n");
			printf("Фермер: %s\n", player->name);
			printf("Время: %d день, %d час\n", current_day, current_hour);
			printf("Содержимое рюкзака:\n");

			for (int i = 0; i < INVENTORY_SIZE; i++) {
				int item_id = player->inventory[i];

				if (item_id >= 0 && item_id < MAX_ITEMS) {
					printf("  Слот %d: %s\n", i, item_names[item_id]);
				}
				else {
					printf("  Слот %d: Неизвестный предмет\n", i);
				}
			}

			printf("---------------------------\n");

			if (save_player_diary(player, current_day, current_hour, item_names)) {
				printf("Запись добавлена в дневник!\n");
			}
			else {
				printf("Не удалось сохранить запись в дневник!\n");
			}

			break;
		}
		case 11: {
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

		case 12:
			adventure(player, &current_day, &current_hour);
			break;

		default:
			printf("Неверный пункт меню. Попробуйте еще раз.\n");
			break;
		}
	}

	return 0;
}
