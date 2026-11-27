#include "adventure.h"
#include "map.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

void adventure(Player* player, int* current_day, int* current_hour)
{
    if (player == NULL)
    {
        printf("Сначала создайте фермера!\n");
        return;
    }

    if (current_day == NULL || current_hour == NULL)
    {
        printf("Ошибка игрового времени!\n");
        return;
    }

    if (player->stamina <= 0)
    {
        printf("Недостаточно выносливости для приключения!\n");
        return;
    }

    if (*current_hour >= 2 && *current_hour < 8)
    {
        printf("После 02:00 нельзя отправляться в приключение! Пора отдохнуть!\n");
        return;
    }

    printf("\n=== БАШНЯ ===\n");
    int height;
    int width;
    int key;

    int player_y = 1;
    int player_x = 1;

    Map* map = NULL;
    int current_floor = 1;

    while (1)
    {
        height = MIN_MAP_SIZE +
            rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

        width = MIN_MAP_SIZE +
            rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

        map = create_map(width, height);

        if (map == NULL)
        {
            printf("Ошибка создания карты!\n");
            return;
        }

        map->floor = current_floor;

        int map_created = 0;
        int stair_y = -1;
        int stair_x = -1;

        for (int attempt = 0; attempt < 100; attempt++)
        {
            generate_map(map);

            player_y = 1;
            player_x = 1;
            map->cells[player_y][player_x] = 'D';

            if (!place_stairs_up(map, &stair_y, &stair_x))
            {
                continue;
            }

            if (possible_path(map, player_y, player_x, stair_y, stair_x))
            {
                map_created = 1;
                break;
            }
        }

        if (!map_created)
        {
            printf("Не удалось создать проходимый этаж!\n");
            free_map(&map);
            return;
        }

        printf("\nЭтаж: %d\n", map->floor);
        printf("Размер карты: %d x %d\n", height, width);

        printf("\nДень: %d | Время: %02d:00\n",
            *current_day, *current_hour);

        printf("Здоровье: %d/%d | Выносливость: %d/%d | Золото: %d\n",
            player->hp, player->max_hp,
            player->stamina, player->max_stamina,
            player->gold);

        printf("Вес: %d/%d | Нагрузка: %s\n",
            player->backpack.current_weight,
            player->backpack.max_capacity,
            player->backpack.load_state == LIGHT ? "Налегке" :
            player->backpack.load_state == NORMAL ? "Норма" : "Перегруз");

        print_map(map, player_y, player_x);
        int next_floor = 0;
        int moves = 0;
        int emergency_evacuation = 0;

        while (1)
        {
            if (emergency_evacuation)
            {
                printf("\nЛестница U разрушена! Нажмите Q для эвакуации!\n");
            }
            else
            {
                printf("\nW A S D | Q - домой\n");
            }

            key = _getwch();

            if (key == 'q' || key == 'Q' ||
                key == 0x0439 || key == 0x0419)
            {
                if (map->cells[player_y][player_x] == 'D' ||
                    emergency_evacuation)
                {
                    free_map(&map);
                    printf("\nВы вернулись домой!\n");
                    return;
                }

                printf("Чтобы вернуться домой, найдите D!\n");
                continue;
            }

            if (emergency_evacuation)
            {
                printf("Нажмите Q для эвакуации!\n");
                continue;
            }

            if (!control_player(map, &player_y, &player_x, key))
            {
                printf("Нельзя пройти в этом направлении!\n");
                continue;
            }

            moves++;
            int stamina_cost = player->backpack.load_state == OVERLOADED ? 3 : 1;
            player->stamina -= stamina_cost;

            if (moves % 5 == 0)
            {
                int stair_collapsed = collapse_floor(map, player_y, player_x);

                if (stair_collapsed && !emergency_evacuation)
                {
                    emergency_evacuation = 1;
                    player->gold /= 2;

                    printf("\nЛестница U разрушена! Необходима эвакуация!\n");
                    printf("Потеряна половина золота.\n");
                }
            }

            if (player->stamina < 0)
            {
                player->stamina = 0;
            }

            if (player->stamina == 0)
            {
                if (!emergency_evacuation)
                {
                    player->gold /= 2;
                    printf("Потеряна половина золота.\n");
                }

                printf("\nВы выбились из сил! Необходима эвакуация!\n");

                free_map(&map);
                return;
            }
            if (!emergency_evacuation &&
                map->cells[player_y][player_x] == 'U')
            {
                next_floor = 1;
                break;
            }

            printf("\nДень: %d | Время: %02d:00\n",
                *current_day, *current_hour);

            printf("Здоровье: %d/%d | Выносливость: %d/%d | Золото: %d\n",
                player->hp, player->max_hp,
                player->stamina, player->max_stamina,
                player->gold);

            print_map(map, player_y, player_x);
        }

        if (next_floor)
        {
            player->stamina -= 5;

            (*current_hour)++;

            if (*current_hour >= 24)
            {
                (*current_day)++;
                *current_hour = 0;
            }

            int evacuation_needed = 0;

            if (*current_hour >= 2 && *current_hour < 8)
            {
                printf("\nНаступило время эвакуации!\n");
                evacuation_needed = 1;
            }

            if (player->stamina <= 0)
            {
                player->stamina = 0;
                printf("\nВы выбились из сил!\n");
                evacuation_needed = 1;
            }

            if (evacuation_needed)
            {
                player->gold /= 2;
                printf("Потеряна половина золота.\n");
                printf("Необходима эвакуация!\n");

                free_map(&map);
                return;
            }

            player->gold += BASE_REWARD * current_floor;

            printf("\nНаграда за этаж: %d золота!\n",
                BASE_REWARD * current_floor);

            current_floor++;
            free_map(&map);
            map = NULL;
        }
    }
}