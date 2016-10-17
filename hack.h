#ifndef HACK_H
#define HACK_H

#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <pthread.h>
#include <curses.h>
#include "sha1/sha1.h"
#include "bsp/BSPParser.hpp"

#include "offsets.h"
#include "memory.h"
#include "esp.h"
#include "signature.h"

static char* keys[12] = {"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12"};

typedef void*(*hack_ptr)(void *);
typedef void*(*hack_ptr_ex)(void *, bool);

typedef struct hack_t {
	bool ex;
	bool enabled;
	char* _name;
	int hotkey;
	hack_ptr ptr;
	hack_ptr_ex ptr_ex;
	pthread_t thread;
} hack_t;

typedef struct entity_t {
	int entity;
	bool dormant;
	bool spotted;
	bool index;
	int team;
	int health;
} entity_t;

typedef struct {
	DWORD client_dll;
	DWORD engine_dll;
	DWORD engine_pointer;
	HANDLE process;
	
	int local_player;
	entity_t entities[64];
	int player_count;
	
	hack_t hack_array[10];
	int num_hacks;
	
	// ncurses
	WINDOW* hackstatus;
	WINDOW* hackinfo;
	
	// other
	char* sha1;
	Valve::BSPParser parser;
	ESP esp;
} Hack;

typedef struct ptr_t {
	int id;
	Hack* hack;
	hack_ptr ptr;
} ptr_t;

void init_hack(Hack*, const char*);
void add_hack(Hack*, hack_ptr, const char*, int);
void add_hack_ex(Hack*, hack_ptr_ex, const char*, int);
void start_hack(Hack*);
void stop_hack(Hack*);

static DWORD find_dll(HANDLE, const char*);
static void get_sha1(Hack*);

static void thread_loop(ptr_t);

#endif /* end of include guard: HACK_H */
