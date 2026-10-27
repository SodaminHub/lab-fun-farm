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


int safe_scan(int* value) { // Проверяю на введение числа, в противном случае возвращаю ошибку и обратно к введению значения.
	if (scanf("%d", value) != 1) {
		printf("Ошибка: Введено не число!\n");
		while (getchar() != '\n'); 
		return 0;
	}
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

	while (1) {
		printf("\n=== ПЕРВЫЕ ШАГИ ВЕСЁЛОГО ФЕРМЕРА ===\n");
		printf(" 0. Выйти из игры\n");
		printf(" 1. Посмотреть на часы\n");
		printf(" 2. Поработать на ферме\n");
		printf(" 3. Проверить инвентарь\n");
		printf(" 4. Положить предмет в инвентарь\n");
		printf(" 5. Выбросить предмет\n");
		printf(" 6. Проверить соседние слоты\n");
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
			for (int i = 0; i < INVENTORY_SIZE; i++) {
				printf("Слот %d: [%d]", i, inventory[i]); // Вывожу индекс слота и ID предмета

				switch (inventory[i]) { // Соотнёс ID предмета и напечатал его название для игрока

				case ID_WOOD:     printf("(Древесина)"); break;

				case ID_STONE:    printf("(Камень)"); break;

				case ID_SEEDS:    printf("(Семена)"); break;

				case ID_HOE:      printf("(Тяпка)"); break;

				case ID_APPLE:    printf("(Яблоко)"); break;

				case ID_SHOVEL:   printf("(Лопата)"); break;

				case ID_IRON:     printf("(Железо)"); break;

				case ID_ROPE:     printf("(Веревка)"); break;

				case ID_COIN:     printf("(Монета)"); break;

				default:          printf("(Пусто)"); break;

				}
				printf("\n");
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
		default:
			printf("Неверный пункт меню. Попробуйте еще раз.\n");
			break;
		}
	}

    return 0;
}