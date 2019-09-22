#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include <math.h>
#include <limits.h>
#include "wc.h"

typedef struct {
    char* key;
    char* value;
} dataItem;

struct wc{
    long size;
    int filledCount;
    dataItem** items;
};

static dataItem deletedDataItem = {NULL, NULL};

static int hash(char* str, int prime, int buckets){
    long hash = 1;
    for (int i = 0; i < strlen(str); i++){
        hash += (long)pow(prime, strlen(str) - (i+1)) * abs(str[i]);
        hash = hash % buckets;
    }
    return (int)hash;
};

static int getHash(char* str, int attempt, int buckets){
    int hashA = hash(str, 151, buckets);
    int hashB = hash(str, 163, buckets);
    return (hashA + (attempt * (hashB + 1))) % buckets;
};

static dataItem* newDataItem(char* inputKey, char* inputValue){
    dataItem* i = malloc(sizeof(dataItem));
    i->key = strdup(inputKey);
    i->value = strdup(inputValue);
    return i;
};

void insert(struct wc* wc, char* key, char* value){
    dataItem* item = newDataItem(key, value);
    int index = getHash(item->key, 0, wc->size);
    dataItem* currItem = wc->items[index];
    int i = 1;
    while(currItem != NULL){
        if (currItem != &deletedDataItem) {
            if (strcmp(currItem->key, key) == 0) {
                free(currItem->key);
                free(currItem->value);
                free(currItem);
                wc->items[index] = item;
                return;
            }
        }
        index = getHash(item->key, i, wc->size);
        currItem = wc->items[index];
        i++;
    }
    wc->items[index] = item;
    wc->filledCount++;
};

void delete(struct wc* wc, char* key) {
    int index = getHash(key, 0, wc->size);
    dataItem* item = wc->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &deletedDataItem) {
            if (strcmp(item->key, key) == 0) {
                free(item->key);
                free(item->value);
                free(item);
                wc->items[index] = &deletedDataItem;
            }
        }
        index = getHash(key, i, wc->size);
        item = wc->items[index];
        i++;
    } 
    wc->filledCount--;
}

char* search(struct wc* wc, char* key) {
    int index = getHash(key, 0, wc->size);
    dataItem* item = wc->items[index];
    int i = 1;
    while (item != NULL && item != &deletedDataItem) {
        if (strcmp(item->key, key) == 0) {
            return item->value;
        }
        index = getHash(key, i, wc->size);
        item = wc->items[index];
        i++;
    } 
    return NULL;
}

struct wc *
wc_init(char *word_array, long size)
{   
    
    struct wc* wc = (struct wc *)malloc(sizeof(struct wc));
    wc->size = 100000;
    wc->filledCount = 0;
    wc->items = calloc((size_t)wc->size, sizeof(dataItem));
    char word[40]= "";
    for(int i = 0; i < size; i++){
            if(isspace(word_array[i]) == 0){
                word[strlen(word)] = word_array[i];
            }
            else{
                char* searchRes = search(wc, word);
                if (word[0] == '\0')
                {
                    continue;
                }
                if(searchRes == NULL){
                    insert(wc, word, "1");
                }
                else{
                    int intVersion = atoi(searchRes);
                    intVersion++;
                    sprintf(searchRes,"%d", intVersion);
                    insert(wc, word, searchRes);
                }
                memset(word,0,strlen(word));
            }

    };
    return wc;
}

void
wc_output(struct wc *wc)
{
    for(int i=0; i < wc->size; i++){
        if(wc->items[i] != NULL){
            printf(wc->items[i]->key);
            printf(":");
            printf(wc->items[i]->value);
            printf("\n");
        }
    }
}

void
wc_destroy(struct wc *wc)
{
    for (int i = 0; i < wc->size; i++){
        dataItem* item = wc->items[i];
        if(item != NULL){
            free(item->key);
            free(item->value);
            free(item);
        }
    }
    free(wc->items);
    free(wc);
}
