#ifndef MAP_H
#define MAP_H

#include "player.h"

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30

#define MIN_OBSTACLE_PERCENT 15
#define MAX_OBSTACLE_PERCENT 30

typedef struct Map {
    char** cells;
    int width;
    int height;
    int floor;
} Map;

typedef enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

Map* create_map(int width, int height);
void free_map(Map** map);
void generate_map(Map* map);
void print_map(const Map* map, int player_y, int player_x);
int control_player(Map* map, int* player_y, int* player_x, int key);
int place_stairs(Map* map, char stair, int excluded_y, int excluded_x);
int place_stairs_up(Map* map, int* stair_y, int* stair_x);
int possible_path(const Map* map, int start_y, int start_x,
    int target_y, int target_x);
int collapse_floor(Map* map, int player_y, int player_x);
int adventure(Player* player, int* current_day, int* current_hour);

#endif
