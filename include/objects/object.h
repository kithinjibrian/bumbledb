#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))

#include "objects/str/str.h"
#include "objects/pool/pool.h"
#include "objects/drop/drop.h"
#include "objects/pool/alloc.h"
#include "objects/clone/clone.h"
#include "objects/print/print.h"
#include "objects/share/share.h"
#include "objects/state/state.h"
#include "objects/utils/utils.h"
#include "objects/visit/visit.h"
#include "objects/equals/equals.h"
#include "objects/pickle/pickle.h"
#include "objects/natives/natives.h"

#endif