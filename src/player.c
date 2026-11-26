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
    int starting_inventory[INVENTORY_SIZE] = {
    6, 4, 0, 0, 3, 1, 1, 8, 2, 9
    };

    memcpy(player->inventory, starting_inventory,
        sizeof(starting_inventory));

    player->hp = BASE_HEALTH;
    player->max_hp = BASE_HEALTH;
    player->stamina = BASE_STAMINA;
    player->max_stamina = BASE_STAMINA;
    player->gold = BASE_GOLD;

    printf("Введите имя фермера: ");

    if (scanf("%31s", player->name) != 1)
    {
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
    printf("Координаты: X=%d, Y=%d\n", player->x, player->y);
    printf("======================================\n");
}