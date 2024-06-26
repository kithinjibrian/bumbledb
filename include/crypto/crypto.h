#pragma once
#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>

#include "fs/vfs/vfs.h"
#include "cache/cache.h"
#include "objects/object.h"

typedef struct crypto_keys_o crypto_keys_o;

typedef struct client_crypto_keys_o
{
	int set;
	unsigned char public_key[crypto_box_PUBLICKEYBYTES];
	unsigned char shared_key[crypto_box_BEFORENMBYTES];
} client_crypto_keys_o;

crypto_keys_o *crypto_get(void);
crypto_keys_o *new_crypto(void);
unsigned char *get_public_key(void);
unsigned char *get_secret_key(void);
unsigned char *get_sign_public_key(void);
unsigned char *get_sign_secret_key(void);

#endif