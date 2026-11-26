
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Map* create_map(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    Map* map = malloc(sizeof(Map));

    if (map == NULL) {
        return NULL;
    }

    map->cells = malloc(height * sizeof(char*));

    if (map->cells == NULL) {
        free(map);
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        map->cells[i] = NULL;
    }

    for (int i = 0; i < height; i++) {
        map->cells[i] = malloc(width * sizeof(char));

        if (map->cells[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(map->cells[j]);
            }

            free(map->cells);
            free(map);
            return NULL;
        }
    }

    map->width = width;
    map->height = height;
    map->floor = 1;

    return map;
}

void free_map(Map** map) {
    if (map == NULL || *map == NULL) {
        return;
    }

    for (int i = 0; i < (*map)->height; i++) {
        free((*map)->cells[i]);
    }

    free((*map)->cells);
    free(*map);
    *map = NULL;
}

void generate_map(Map* map) {
    if (map == NULL || map->cells == NULL) {
        return;
    }

    int obstacle_percent = MIN_OBSTACLE_PERCENT +
        rand() % (MAX_OBSTACLE_PERCENT - MIN_OBSTACLE_PERCENT + 1);

    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            if (i == 0 || i == map->height - 1 ||
                j == 0 || j == map->width - 1) {
                map->cells[i][j] = '#';
            }
            else {
                int chance = rand() % 100;

                if (chance < obstacle_percent) {
                    map->cells[i][j] = (rand() % 2 == 0) ? 'T' : 'R';
                }
                else {
                    map->cells[i][j] = '.';
                }
            }
        }
    }
}

void print_map(const Map* map, int player_y, int player_x) {
    if (map == NULL || map->cells == NULL) {
        return;
    }

    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {

            if (i == player_y && j == player_x) {
                printf("@ ");
            }
            else {
                printf("%c ", map->cells[i][j]);
            }
        }
        printf("\n");
    }
}

int control_player(Map* map, int* player_y, int* player_x, int key) {
    if (map == NULL || map->cells == NULL ||
        player_y == NULL || player_x == NULL) {
        return 0;
    }

    int new_y = *player_y;
    int new_x = *player_x;

    switch (key) {
    case 'w':
    case 'W':
    case L'ц':
    case L'Ц':
        new_y--;
        break;

    case 's':
    case 'S':
    case L'ы':
    case L'Ы':
        new_y++;
        break;

    case 'a':
    case 'A':
    case L'ф':
    case L'Ф':
        new_x--;
        break;

    case 'd':
    case 'D':
    case L'в':
    case L'В':
        new_x++;
        break;

    default:
        return 0;
    }

    if (new_y < 0 || new_y >= map->height ||
        new_x < 0 || new_x >= map->width) {
        return 0;
    }

    if (map->cells[new_y][new_x] == '#' ||
        map->cells[new_y][new_x] == 'T' ||
        map->cells[new_y][new_x] == 'R' ||
        map->cells[new_y][new_x] == ' ') {
        return 0;
    }

    *player_y = new_y;
    *player_x = new_x;

    return 1;
}

int place_stairs(Map* map, char stair, int excluded_y, int excluded_x) {
    if (map == NULL || map->cells == NULL) {
        return 0;
    }

    int free_cells = 0;

    for (int i = 1; i < map->height - 1; i++) {
        for (int j = 1; j < map->width - 1; j++) {
            if (map->cells[i][j] == '.' &&
                !(i == excluded_y && j == excluded_x)) {
                free_cells++;
            }
        }
    }

    if (free_cells == 0) {
        return 0;
    }

    int target = rand() % free_cells;

    for (int i = 1; i < map->height - 1; i++) {
        for (int j = 1; j < map->width - 1; j++) {
            if (map->cells[i][j] == '.' &&
                !(i == excluded_y && j == excluded_x)) {

                if (target == 0) {
                    map->cells[i][j] = stair;
                    return 1;
                }

                target--;
            }
        }
    }

    return 0;
}

int place_stairs_up(Map* map, int* stair_y, int* stair_x) {
    if (map == NULL || map->cells == NULL ||
        stair_y == NULL || stair_x == NULL) {
        return 0;
    }

    int free_cells = 0;

    for (int i = 1; i < map->height - 1; i++) {
        for (int j = 1; j < map->width - 1; j++) {
            if (map->cells[i][j] == '.') {
                free_cells++;
            }
        }
    }

    if (free_cells == 0) {
        return 0;
    }

    int target = rand() % free_cells;

    for (int i = 1; i < map->height - 1; i++) {
        for (int j = 1; j < map->width - 1; j++) {
            if (map->cells[i][j] == '.') {

                if (target == 0) {
                    map->cells[i][j] = 'U';
                    *stair_y = i;
                    *stair_x = j;
                    return 1;
                }

                target--;
            }
        }
    }

    return 0;
}

int possible_path(const Map* map, int start_y, int start_x,
    int target_y, int target_x) {

    if (map == NULL || map->cells == NULL) {
        return 0;
    }

    int height = map->height;
    int width = map->width;

    if (start_y < 0 || start_y >= height ||
        start_x < 0 || start_x >= width ||
        target_y < 0 || target_y >= height ||
        target_x < 0 || target_x >= width) {
        return 0;
    }

    int** visited = calloc(height, sizeof(int*));
    int* queue_y = malloc(height * width * sizeof(int));
    int* queue_x = malloc(height * width * sizeof(int));

    if (visited == NULL || queue_y == NULL || queue_x == NULL) {
        free(visited);
        free(queue_y);
        free(queue_x);
        return 0;
    }

    int allocated = 0;

    for (int i = 0; i < height; i++) {
        visited[i] = calloc(width, sizeof(int));

        if (visited[i] == NULL) {
            for (int j = 0; j < allocated; j++) {
                free(visited[j]);
            }

            free(visited);
            free(queue_y);
            free(queue_x);
            return 0;
        }

        allocated++;
    }

    int front = 0;
    int rear = 0;

    queue_y[rear] = start_y;
    queue_x[rear] = start_x;
    rear++;

    visited[start_y][start_x] = 1;

    int dy[4] = { -1, 1, 0, 0 };
    int dx[4] = { 0, 0, -1, 1 };

    int result = 0;

    while (front < rear) {
        int y = queue_y[front];
        int x = queue_x[front];
        front++;

        if (y == target_y && x == target_x) {
            result = 1;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int new_y = y + dy[i];
            int new_x = x + dx[i];

            if (new_y < 0 || new_y >= height ||
                new_x < 0 || new_x >= width) {
                continue;
            }

            if (visited[new_y][new_x]) {
                continue;
            }

            if (map->cells[new_y][new_x] == '#' ||
                map->cells[new_y][new_x] == 'T' ||
                map->cells[new_y][new_x] == 'R') {
                continue;
            }

            visited[new_y][new_x] = 1;

            queue_y[rear] = new_y;
            queue_x[rear] = new_x;
            rear++;
        }
    }

    for (int i = 0; i < height; i++) {
        free(visited[i]);
    }

    free(visited);
    free(queue_y);
    free(queue_x);

    return result;
}

int collapse_floor(Map* map, int player_y, int player_x) {
    if (map == NULL || map->cells == NULL) {
        return 0;
    }

    int layer = map->floor - 1;

    int top = 1 + layer;
    int bottom = map->height - 2 - layer;
    int left = 1 + layer;
    int right = map->width - 2 - layer;

    int stair_collapsed = 0;

    if (top > bottom || left > right) {
        return 0;
    }

    for (int i = top; i <= bottom; i++) {
        for (int j = left; j <= right; j++) {

            if (i != top && i != bottom &&
                j != left && j != right) {
                continue;
            }

            if (i == player_y && j == player_x) {
                continue;
            }

            if (map->cells[i][j] == 'U') {
                stair_collapsed = 1;
            }

            map->cells[i][j] = ' ';
        }
    }

    return stair_collapsed;
}