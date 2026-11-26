#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "player.h"


#define BASE_REWARD 20

int safe_scan(const char* format, void* value);
void save_to_diary(const char* message);
int find_item_id(const char* name);
int value_inventory(const int* inv, const char catalog[MAX_ITEMS][MAX_NAME_LEN]);
void load_item_catalog(char catalog[MAX_ITEMS][MAX_NAME_LEN], const char* filename);

#endif
