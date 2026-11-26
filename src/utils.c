#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int safe_scan(const char* format, void* value)
{
    if (format == NULL || value == NULL)
    {
        return 0;
    }

    int result = scanf(format, value);

    if (result != 1)
    {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) // Очищаю некорректный ввод
        {
            
        }

        return 0;
    }

    return 1;
}

void save_to_diary(const char* message)
{
    if (message == NULL)
    {
        return;
    }

    FILE* file = fopen("diary.txt", "a");

    if (file == NULL)
    {
        printf("Не удалось открыть дневник!\n");
        return;
    }

    fprintf(file, "%s\n", message);

    fclose(file);
}

int find_item_id(const char* name)
{
    if (name == NULL)
    {
        return ID_EMPTY;
    }

    FILE* file = fopen("items.txt", "r");

    if (file == NULL)
    {
        printf("Не удалось открыть items.txt!\n");
        return ID_EMPTY;
    }

    int id;
    char item_name[100];

    while (fscanf(file, "%d ", &id) == 1)
    {
        if (fgets(item_name, sizeof(item_name), file) == NULL)
        {
            break;
        }

        item_name[strcspn(item_name, "\r\n")] = '\0';

        if (strcmp(item_name, name) == 0)
        {
            fclose(file);
            return id;
        }
    }

    fclose(file);
    return ID_EMPTY;
}

void load_item_catalog(char catalog[MAX_ITEMS][MAX_NAME_LEN], const char* filename)
{
    if (catalog == NULL || filename == NULL)
    {
        return;
    }

    FILE* file = fopen(filename, "r");

    if (file != NULL)
    {
        int temp_id;
        char temp_name[MAX_NAME_LEN];

        while (fscanf(file, "%d %31s", &temp_id, temp_name) == 2)
        {
            if (temp_id >= 0 && temp_id < MAX_ITEMS)
            {
                strcpy(catalog[temp_id], temp_name);
            }
        }

        fclose(file);
    }
    else
    {
        printf("Предупреждение: файл %s не найден! Загружены стандартные названия.\n", filename);

        strcpy(catalog[ID_EMPTY], "Пусто");
        strcpy(catalog[ID_WOOD], "Древесина");
        strcpy(catalog[ID_STONE], "Камень");
        strcpy(catalog[ID_SEEDS], "Семена");
        strcpy(catalog[ID_HOE], "Тяпка");
        strcpy(catalog[ID_APPLE], "Яблоко");
        strcpy(catalog[ID_SHOVEL], "Лопата");
        strcpy(catalog[ID_IRON], "Железо");
        strcpy(catalog[ID_ROPE], "Верёвка");
        strcpy(catalog[ID_CHAINSAW], "Бензопила");
    }
}

int value_inventory(const int* inv, const char catalog[MAX_ITEMS][MAX_NAME_LEN]) {
    FILE* f_out = fopen("output.txt", "w");
    if (f_out == NULL) return 0;

    int total_sum = 0;
    int max_price = -1;
    char most_expensive_item[MAX_NAME_LEN] = "Нет предметов";

    fprintf(f_out, "=== ОЦЕНКА СТОИМОСТИ ПРЕДМЕТОВ РЮКЗАКА ===\n");

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int current_id = inv[i];
        int item_price = 0;

        if (current_id != ID_EMPTY) { // Если слот всёж не пустой, ищу его цену в прайс-листе торговца
            FILE* f_in = fopen("input.txt", "r");
            if (f_in != NULL) {
                char p_name[MAX_NAME_LEN];
                int p_price;

                while (fscanf(f_in, "%31s %d", p_name, &p_price) == 2) { 	// Печатаю цены у торговца
                    if (strcmp(p_name, catalog[current_id]) == 0) {
                        item_price = p_price;
                        break;
                    }
                }
                fclose(f_in);
            }
        }

        fprintf(f_out, "Слот %d: %s - %d монет\n", i, catalog[current_id], item_price); // Записываю стоимость конкретного слота
        total_sum += item_price;

        if (current_id != ID_EMPTY && item_price > max_price) { // Ищу самый дорогой предмет
            max_price = item_price;
            strcpy(most_expensive_item, catalog[current_id]);
        }
    }

    fprintf(f_out, "-----------------------------------\n");
    fprintf(f_out, "Самый дорогой предмет: %s (%d монет)\n", most_expensive_item, max_price > -1 ? max_price : 0);
    fprintf(f_out, "Общая стоимость инвентаря: %d монет\n", total_sum);
    fprintf(f_out, "===================================\n");

    fclose(f_out);
    return 1;
}