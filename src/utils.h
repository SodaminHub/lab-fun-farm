#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_ITEMS 10
#define MAX_NAME_LEN 32
#define INVENTORY_SIZE 10

#define BASE_REWARD 20

int safe_scan(const char* format, void* value);
void save_to_diary(const char* message);
int find_item_id(const char* name);
int load_item_catalog(const char* filename);

#endif
