#pragma once
#ifndef BUMBLE_H
#define BUMBLE_H

#include "cache/cache.h"
#include "crypto/crypto.h"
#include "objects/object.h"
#include "proto/packet.pb-c.h"
#include "proto/message.pb-c.h"
#include "server/server/server.h"

typedef array_o bumble_o;
typedef htable_o request_o;
typedef htable_o response_o;

typedef void *(*fun_middleware_t)(bumble_o *, request_o *, response_o *);

void bumble(client_o *client);

/**/
void bumble_print();
void bumble_use(fun_generic_t middleware);
void bumble_action(const char *action, int n, ...);
bool bumble_next(bumble_o *self, request_o *request, response_o *response);

/**/
void bumble_routes();

/**/
bool send_signed(request_o *req, response_o *res, uint8_t *bytes);
bool send_unencrypted(request_o *req, response_o *res, uint8_t *bytes);
bool send_encrypted(request_o *req, response_o *res, uint8_t *bytes);
bool send_encrypted2(request_o *req, response_o *res, client_crypto_keys_o *client_keys, uint8_t *bytes);

bool bee_bytes(request_o *req, response_o *res, uint8_t *bytes);
bool bee_send(request_o *req, response_o *res, string_o *message);

bool bee_status(response_o *res, int status);

uint8_t *ptoken(htable_o *token);

#endif