#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Player* create_player(void)
{
    Player* player = malloc(sizeof(Player));

    if (player == NULL)
    {
        printf("Ошибка выделения памяти для игрока!\n");
        return NULL;
    }

    memset(player, 0, sizeof(Player));

    int starting_inventory[INVENTORY_SIZE] = {6, 4, 0, 0, 3, 1, 1, 8, 2, 9};

    memcpy(player->backpack.slots, starting_inventory,
        sizeof(starting_inventory));

    player->backpack.max_capacity = BASE_BACKPACK_CAPACITY;
    update_backpack(&player->backpack);

    player->hp = BASE_HEALTH;
    player->max_hp = BASE_HEALTH;
    player->stamina = BASE_STAMINA;
    player->max_stamina = BASE_STAMINA;
    player->gold = BASE_GOLD;

    printf("Введите имя фермера: ");

    if (scanf_s("%31s", player->name, (unsigned)_countof(player->name)) != 1)
    {
        printf("Ошибка ввода имени фермера!\n");
        free(player);
        return NULL;
    }

    return player;
}

void free_player(Player** player)
{
    if (player == NULL || *player == NULL)
    {
        return;
    }

    printf("Память игрока освобождена. Адрес: %p\n", (void*)*player);

    free(*player);
    *player = NULL;
}

void show_player(const Player* player)
{
    if (player == NULL)
    {
        printf("Игрок ещё не создан!\n");
        return;
    }

    printf("\n========== КАРТОЧКА ФЕРМЕРА ==========\n");
    printf("Имя: %s\n", player->name);
    printf("Здоровье: %d/%d\n", player->hp, player->max_hp);
    printf("Выносливость: %d/%d\n", player->stamina, player->max_stamina);
    printf("Золото: %d\n", player->gold);
    printf("Вес рюкзака: %d/%d\n",
        player->backpack.current_weight,
        player->backpack.max_capacity);

    printf("Состояние рюкзака: %s\n",
        player->backpack.load_state == LIGHT ? "Налегке" :
        player->backpack.load_state == NORMAL ? "Норма" : "Перегруз");
    printf("Координаты: X=%d, Y=%d\n", player->x, player->y);
    printf("======================================\n");
}

int get_item_weight(int item_id)
{
    const int item_weights[MAX_ITEMS] = {
        0,  // Пусто
        2,  // Древесина
        4,  // Камень
        1,  // Семена
        3,  // Тяпка
        1,  // Яблоко
        4,  // Лопата
        3,  // Железо
        1,  // Верёвка
        8   // Бензопила
    };

    if (item_id < 0 || item_id >= MAX_ITEMS)
    {
        return 0;
    }

    return item_weights[item_id];
}

void update_backpack(Backpack* backpack)
{
    if (backpack == NULL)
    {
        return;
    }

    backpack->current_weight = 0;

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        backpack->current_weight += get_item_weight(backpack->slots[i]);
    }

    if (backpack->current_weight <= backpack->max_capacity / 2)
    {
        backpack->load_state = LIGHT;
    }
    else if (backpack->current_weight <= backpack->max_capacity)
    {
        backpack->load_state = NORMAL;
    }
    else
    {
        backpack->load_state = OVERLOADED;
    }
}