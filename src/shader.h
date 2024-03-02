#ifndef __SHADER_H__
#define __SHADER_H__

#include <stdint.h>

#include "error.h"

typedef uint32_t shader;

mrerror shader_new(shader *s, const char *vert, const char *frag);

mrerror shader_load(shader *s, const char *vert_path, const char *frag_path);

#endif