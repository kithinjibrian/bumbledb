#include "crypto/crypto.h"

struct sign_keys_o
{
	unsigned char public_key[crypto_sign_PUBLICKEYBYTES];
	unsigned char secret_key[crypto_sign_SECRETKEYBYTES];
};

static sign_keys_o *sign_keys_g = NULL;

sign_keys_o *sign_keys_get()
{
	if (sign_keys_g == NULL)
	{
		sign_keys_g = new_sign_keys();
		pool_globals_add(sign_keys_g);
	}
	return sign_keys_g;
}

int sign_key_write(const char *filename, unsigned char *key, size_t length)
{
	SMART file_o *key_file = f_open(filename, "wb");

	fwrite(key, length, 1, f_get(key_file));

	return 1;
}

int sign_key_read(const char *filename, unsigned char *key, size_t length)
{
	SMART file_o *key_file = f_open(filename, "rb");
	if (is_error(key_file))
	{
		DROP(key_file);
		return 0;
	}

	fread(key, length, 1, f_get(key_file));

	return 1;
}

sign_keys_o *new_sign_keys(void)
{
	sign_keys_o *sign_keys;

	sign_keys = alloc(1, sizeof(sign_keys_o), SERVER_CRYPTO_KEYS_O);

	int a = sign_key_read("public_key.bin", sign_keys->public_key, crypto_sign_PUBLICKEYBYTES);
	int b = sign_key_read("secret_key.bin", sign_keys->secret_key, crypto_sign_SECRETKEYBYTES);

	if (a == 0 || b == 0)
	{
		crypto_sign_keypair(sign_keys->public_key, sign_keys->secret_key);
		sign_key_write("public_key.bin", sign_keys->public_key, crypto_sign_PUBLICKEYBYTES);
		sign_key_write("secret_key.bin", sign_keys->secret_key, crypto_sign_SECRETKEYBYTES);
	}

	return sign_keys;
}

unsigned char *get_sign_public_key(void)
{
	sign_keys_o *sign_keys = sign_keys_get();
	return sign_keys->public_key;
}

unsigned char *get_sign_secret_key(void)
{
	sign_keys_o *sign_keys = sign_keys_get();
	return sign_keys->secret_key;
}