#pragma once
#ifndef VISIT_H
#define VISIT_H

#include "objects/pool/pool.h"

typedef void (*fun_process2_t)(object_header_t *, object_o, object_o);

void post_visit(object_o object, fun_process_t fun_process);

#endif