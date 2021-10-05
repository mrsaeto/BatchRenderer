#ifndef SALAMANDER_BASIC_H
#define SALAMANDER_BASIC_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

//TODO: get a more accurate approximation of pi
#define PI 3.14159265359
#define DEGREES_TO_RADIANS(d) ((d)*(PI / 180))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <cglm/cglm.h>

#endif