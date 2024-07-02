#pragma once
#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>

#include "fs/vfs/vfs.h"
#include "cache/cache.h"
#include "objects/object.h"

typedef struct sign_keys_o sign_keys_o;

typedef struct diffie_keys_o
{
	int client_diffie_set;
	unsigned char client_public_key[crypto_box_PUBLICKEYBYTES];
	unsigned char client_shared_key[crypto_box_BEFORENMBYTES];

	int server_diffie_set;
	unsigned char server_public_key[crypto_box_PUBLICKEYBYTES];
	unsigned char server_secret_key[crypto_box_SECRETKEYBYTES];
} diffie_keys_o;

sign_keys_o *sign_keys_get(void);
sign_keys_o *new_sign_keys(void);

unsigned char *get_sign_public_key(void);
unsigned char *get_sign_secret_key(void);

#endif