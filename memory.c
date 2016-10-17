#include "memory.h"

//// read

// base types
int   read_int   (HANDLE process, DWORD loc) {
	int _out;
	ReadProcessMemory(process, loc, &_out, sizeof(int), NULL);
	return _out;
}
float read_float (HANDLE process, DWORD loc) {
	float _out;
	ReadProcessMemory(process, loc, &_out, sizeof(float), NULL);
	return _out;
}
bool  read_bool  (HANDLE process, DWORD loc) {
	bool _out;
	ReadProcessMemory(process, loc, &_out, sizeof(bool), NULL);
	return _out;
}
DWORD read_dword (HANDLE process, DWORD loc) {
	DWORD _out;
	ReadProcessMemory(process, loc, &_out, sizeof(DWORD), NULL);
	return _out;
}

// vectors
void  read_vec2 (HANDLE process, DWORD loc, float* _out) {
	ReadProcessMemory(process, loc, _out, sizeof(float)*2, NULL);
}
void  read_vec3 (HANDLE process, DWORD loc, float* _out) {
	ReadProcessMemory(process, loc, _out, sizeof(float)*3, NULL);
}
void  read_vec4 (HANDLE process, DWORD loc, float _out[4]) {
	ReadProcessMemory(process, loc, _out, sizeof(float)*4, NULL);
}
void read_mat4 (HANDLE process, DWORD loc, float _out[4][4]) {
	ReadProcessMemory(process, loc, _out, sizeof(float)*4*4, NULL);
}

//// write

// base types
void write_int(HANDLE process, DWORD loc, int in) {
	WriteProcessMemory(process, loc, &in, sizeof(int), NULL);
}

void write_float(HANDLE process, DWORD loc, float in) {
	WriteProcessMemory(process, loc, &in, sizeof(float), NULL);
}

void write_bool(HANDLE process, DWORD loc, bool in) {
	WriteProcessMemory(process, loc, &in, sizeof(bool), NULL);
}

void write_dword(HANDLE process, DWORD loc, DWORD in) {
	WriteProcessMemory(process, loc, &in, sizeof(DWORD), NULL);
}

// vectors
void write_vec2(HANDLE process, DWORD loc, float vector[2])
{
	WriteProcessMemory(process, loc, vector, sizeof(float)*2, NULL);
}

void write_vec3(HANDLE process, DWORD loc, float vector[3])
{
	WriteProcessMemory(process, loc, vector, sizeof(float)*3, NULL);
}

void write_vec4(HANDLE process, DWORD loc, float vector[4])
{
	WriteProcessMemory(process, loc, vector, sizeof(float)*4, NULL);
}
