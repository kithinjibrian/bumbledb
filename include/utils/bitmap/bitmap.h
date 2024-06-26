#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <stdint.h>

int bit_get(uint8_t bitmap[], int index);
void bit_set(uint8_t bitmap[], int index);
void bit_print(uint8_t bitmap[], int size);
void bit_clear(uint8_t bitmap[], int index);
int bit_search(uint8_t *bitmap, int size, int value);

#endif