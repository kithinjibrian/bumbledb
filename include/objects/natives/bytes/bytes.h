#pragma once
#ifndef BYTES_H
#define BYTES_H

#include <stdarg.h>
#include <stdint.h>

uint8_t *new_bytes(int length);
uint8_t *bytes_from(void *buffer, int length);
char *bytes_to_hex(uint8_t *bytes, int length);
char *bytes_from_hex(uint8_t *hex, int length);

#endif