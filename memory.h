// Copyright (c) 2016 Lumaio All Rights Reserved.

#ifndef MEMORY_H
#define MEMORY_H

#include "hack.h"

//// read

// base types
int   read_int   (HANDLE, DWORD);
float read_float (HANDLE, DWORD);
bool  read_bool  (HANDLE, DWORD);
DWORD read_dword (HANDLE, DWORD);

// vectors
void  read_vec2 (HANDLE, DWORD, float*);
void  read_vec3 (HANDLE, DWORD, float*);
void  read_vec4 (HANDLE, DWORD, float[4]);
void  read_mat4 (HANDLE, DWORD, float[4][4]);


//// write

// base types
void write_int   (HANDLE, DWORD, int);
void write_float (HANDLE, DWORD, float);
void write_bool  (HANDLE, DWORD, bool);
void write_dword (HANDLE, DWORD, DWORD);

// vectors
void write_vec2 (HANDLE, DWORD, float[2]);
void write_vec3 (HANDLE, DWORD, float[3]);
void write_vec4 (HANDLE, DWORD, float[4]);
void write_mat4 (HANDLE, DWORD, float**); // not needed?

#endif /* end of include guard: MEMORY_H */
