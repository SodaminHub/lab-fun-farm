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


int find_item_id(const char catalog[MAX_ITEMS][MAX_NAME_LEN], const char* name)
{
    if (catalog == NULL || name == NULL) {
        return -1;
    }

    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(catalog[i], name) == 0) {
            return i;
        }
    }

    return -1;
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

        while (fscanf_s(file, "%d %31s",
            &temp_id, temp_name, (unsigned)_countof(temp_name)) == 2)
        {
            if (temp_id >= 0 && temp_id < MAX_ITEMS)
            {
                strcpy_s(catalog[temp_id], MAX_NAME_LEN, temp_name);
            }
        }

        fclose(file);
    }
    else
    {
        printf("Предупреждение: файл %s не найден! Загружены стандартные названия.\n", filename);

        strcpy_s(catalog[ID_EMPTY], MAX_NAME_LEN, "Пусто");
        strcpy_s(catalog[ID_WOOD], MAX_NAME_LEN, "Древесина");
        strcpy_s(catalog[ID_STONE], MAX_NAME_LEN, "Камень");
        strcpy_s(catalog[ID_SEEDS], MAX_NAME_LEN, "Семена");
        strcpy_s(catalog[ID_HOE], MAX_NAME_LEN, "Тяпка");
        strcpy_s(catalog[ID_APPLE], MAX_NAME_LEN, "Яблоко");
        strcpy_s(catalog[ID_SHOVEL], MAX_NAME_LEN, "Лопата");
        strcpy_s(catalog[ID_IRON], MAX_NAME_LEN, "Железо");
        strcpy_s(catalog[ID_ROPE], MAX_NAME_LEN, "Верёвка");
        strcpy_s(catalog[ID_CHAINSAW], MAX_NAME_LEN, "Бензопила");
    }
}
int value_inventory(const int* inv, const char catalog[MAX_ITEMS][MAX_NAME_LEN])
{
    if (inv == NULL || catalog == NULL) {
        return 0;
    }

    FILE* f_out = fopen("output.txt", "w");
    if (f_out == NULL) {
        return 0;
    }

    int total_sum = 0;
    int max_price = -1;
    char most_expensive_item[MAX_NAME_LEN] = "Нет предметов";

    fprintf(f_out, "=== ОЦЕНКА СТОИМОСТИ ПРЕДМЕТОВ РЮКЗАКА ===\n");

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int current_id = inv[i];

        if (current_id < 0 || current_id >= MAX_ITEMS) {
            fprintf(f_out, "Слот %d: Некорректный ID предмета\n", i);
            continue;
        }

        int item_price = 0;

        if (current_id != ID_EMPTY) {
            FILE* f_in = fopen("input.txt", "r");

            if (f_in != NULL) {
                char p_name[MAX_NAME_LEN];
                int p_price;

                while (fscanf_s(f_in, "%31s %d",
                    p_name, (unsigned)_countof(p_name), &p_price) == 2) {

                    if (strcmp(p_name, catalog[current_id]) == 0) {
                        item_price = p_price;
                        break;
                    }
                }

                fclose(f_in);
            }
        }

        fprintf(f_out, "Слот %d: %s - %d монет\n",
            i, catalog[current_id], item_price);

        total_sum += item_price;

        if (current_id != ID_EMPTY && item_price > max_price) {
            max_price = item_price;

            strcpy_s(
                most_expensive_item,
                MAX_NAME_LEN,
                catalog[current_id]
            );
        }
    }

    fprintf(f_out, "-----------------------------------\n");
    fprintf(f_out, "Самый дорогой предмет: %s (%d монет)\n",
        most_expensive_item, max_price > -1 ? max_price : 0);

    fprintf(f_out, "Общая стоимость инвентаря: %d монет\n", total_sum);
    fprintf(f_out, "===================================\n");

    fclose(f_out);
    return 1;
}

int save_player_diary(
    const Player* player,
    int day,
    int hour,
    const char catalog[MAX_ITEMS][MAX_NAME_LEN])
{
    if (player == NULL || catalog == NULL) {
        return 0;
    }

    FILE* file = fopen("diary.txt", "a");

    if (file == NULL) {
        return 0;
    }

    fprintf(file, "=== ЗАПИСЬ В ДНЕВНИКЕ ===\n");
    fprintf(file, "Фермер: %s\n", player->name);
    fprintf(file, "Время: %d день, %d час\n", day, hour);
    fprintf(file, "Содержимое рюкзака:\n");

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int item_id = player->inventory[i];

        if (item_id >= 0 && item_id < MAX_ITEMS) {
            fprintf(file, "  Слот %d: %s\n", i, catalog[item_id]);
        }
    }

    fprintf(file, "=========================\n\n");

    fclose(file);
    return 1;
}