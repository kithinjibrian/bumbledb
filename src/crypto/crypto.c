#include "crypto/crypto.h"

struct crypto_keys_o
{
	unsigned char public_key[crypto_box_PUBLICKEYBYTES];
	unsigned char secret_key[crypto_box_SECRETKEYBYTES];
	unsigned char sign_public_key[crypto_sign_PUBLICKEYBYTES];
	unsigned char sign_secret_key[crypto_sign_SECRETKEYBYTES];
};

static crypto_keys_o *crypto_keys_g = NULL;

crypto_keys_o *crypto_get()
{
	if (crypto_keys_g == NULL)
	{
		crypto_keys_g = new_crypto();
		pool_globals_add(crypto_keys_g);
	}
	return crypto_keys_g;
}

crypto_keys_o *new_crypto(void)
{
	crypto_keys_o *crypto_keys = NULL;

	crypto_keys = alloc(1, sizeof(crypto_keys_o), SERVER_CRYPTO_KEYS_O);

	SMART file_o *file = f_open("crypto-keys.bin", "rb");

	if (is_error(file))
	{
		DROP(file);
		file = f_open("crypto-keys.bin", "wb");

		crypto_box_keypair(crypto_keys->public_key, crypto_keys->secret_key);
		crypto_sign_keypair(crypto_keys->sign_public_key, crypto_keys->sign_secret_key);

		fwrite(crypto_keys, 1, sizeof(crypto_keys_o), f_get(file));
	}
	else
	{
		fread(crypto_keys, 1, sizeof(crypto_keys_o), f_get(file));
	}

	return crypto_keys;
}

unsigned char *get_public_key(void)
{
	crypto_keys_o *crypto_keys = crypto_get();
	return crypto_keys->public_key;
}

unsigned char *get_secret_key(void)
{
	crypto_keys_o *crypto_keys = crypto_get();
	return crypto_keys->secret_key;
}

unsigned char *get_sign_public_key(void)
{
	crypto_keys_o *crypto_keys = crypto_get();
	return crypto_keys->sign_public_key;
}

unsigned char *get_sign_secret_key(void)
{
	crypto_keys_o *crypto_keys = crypto_get();
	return crypto_keys->sign_secret_key;
}