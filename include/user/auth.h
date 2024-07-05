#pragma once
#ifndef AUTH_H
#define AUTH_H

#include "quill/quill.h"
#include "objects/object.h"
#include "proto/user.pb-c.h"
#include "server/bumblebee/bumble.h"

void user_routes(void);

bool is_authn(bumble_o *self, request_o *req, response_o *res);
bool user_log_check(bumble_o *self, request_o *req, response_o *res);
bool user_reg_check(bumble_o *self, request_o *req, response_o *res);

#endif