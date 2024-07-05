#pragma once
#ifndef QUILL_H
#define QUILL_H

#include "objects/object.h"

array_o *quill_read(file_o *file);
int quill_write(file_o *file, object_o object);

#endif