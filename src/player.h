
#ifndef PLAYER_H
#define PLAYER_H

#define MAX_NAME_LEN 32
#define INVENTORY_SIZE 10

#define MAX_ITEMS 10

typedef enum ItemId {
    ID_EMPTY = 0,
    ID_WOOD,
    ID_STONE,
    ID_SEEDS,
    ID_HOE,
    ID_APPLE,
    ID_SHOVEL,
    ID_IRON,
    ID_ROPE,
    ID_CHAINSAW
} ItemId;

#define BASE_HEALTH 100
#define BASE_STAMINA 50
#define BASE_GOLD 100

typedef struct Player {
    char name[MAX_NAME_LEN];

    int hp;
    int max_hp;

    int stamina;
    int max_stamina;

    int gold;

    int x;
    int y;

    int inventory[INVENTORY_SIZE];
} Player;

Player* create_player(void);
void free_player(Player** player);
void show_player(const Player* player);

#endif

