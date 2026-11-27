
#ifndef PLAYER_H
#define PLAYER_H

#define MAX_NAME_LEN 32
#define INVENTORY_SIZE 10

#define MAX_ITEMS 10

#define BASE_BACKPACK_CAPACITY 35
#define POUCH_CAPACITY_BONUS 10
#define POUCH_PRICE 50


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

typedef enum LoadState {
    LIGHT,
    NORMAL,
    OVERLOADED
} LoadState;

typedef struct Backpack {
    int slots[INVENTORY_SIZE];
    int current_weight;
    int max_capacity;
    LoadState load_state;
} Backpack;

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

    Backpack backpack;
} Player;

Player* create_player(void);
void free_player(Player** player);
void show_player(const Player* player);

int get_item_weight(int item_id);
void update_backpack(Backpack* backpack);

#endif

