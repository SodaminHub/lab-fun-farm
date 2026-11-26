
#ifndef PLAYER_H
#define PLAYER_H

#define MAX_NAME_LEN 32
#define INVENTORY_SIZE 10

#define MAX_ITEMS 10

#define ID_EMPTY 0
#define ID_WOOD 1
#define ID_STONE 2
#define ID_SEEDS 3
#define ID_HOE 4
#define ID_APPLE 5
#define ID_SHOVEL 6
#define ID_IRON 7
#define ID_ROPE 8
#define ID_COIN 9

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

