#include "hack.h"

void init_hack(Hack* hack, const char* exe)
{
	hack->num_hacks = 0;
	hack->player_count = 0;
	
	// locate EXE
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (Process32First(snapshot, &entry) == TRUE) {
		do {
			if (strcmp(entry.szExeFile, exe) == 0) {
				hack->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				break;
	}   } while (Process32Next(snapshot, &entry) == TRUE); }
	
	// find engine/client dll
	hack->client_dll = find_dll(hack->process, "client.dll");
	hack->engine_dll = find_dll(hack->process, "engine.dll");
	hack->engine_pointer = read_dword(hack->process, hack->engine_dll+EnginePointer);
	// if (!hack->client_dll || !hack->engine_dll || !hack->engine_pointer) return;
	
	// sha1
	get_sha1(hack);
	init_esp(&hack->esp);
}

void add_hack(Hack* hack, hack_ptr ptr, const char* name, int hotkey)
{
	hack->hack_array[hack->num_hacks].ex = FALSE;
	hack->hack_array[hack->num_hacks]._name = name;
	hack->hack_array[hack->num_hacks].enabled = FALSE;
	hack->hack_array[hack->num_hacks].hotkey = hotkey;
	hack->hack_array[hack->num_hacks].ptr = ptr;
	hack->num_hacks++;
	
	RegisterHotKey(NULL, 1, NULL, hotkey);
	
}

void add_hack_ex(Hack* hack, hack_ptr_ex ptr, const char* name, int hotkey)
{
	hack->hack_array[hack->num_hacks].ex = TRUE;
	hack->hack_array[hack->num_hacks]._name = name;
	hack->hack_array[hack->num_hacks].enabled = FALSE;
	hack->hack_array[hack->num_hacks].hotkey = hotkey;
	hack->hack_array[hack->num_hacks].ptr_ex = ptr;
	hack->num_hacks++;
	
	RegisterHotKey(NULL, 1, NULL, hotkey);
}

void start_hack(Hack* hack)
{
	// DWORD local_player = FindPattern("\xB8????\x84\xC9\x74\x06\x21\x05????\xF6\x05?????\x74\x03\x83\xCE\x04", "x???xxxxxx????xx?????xxxxx", hack->client_dll, GetModuleSize("client.dll"), 0x2);
	// BYTE sig[] = {0x0F, 0x5A, 0xC0, 0x8B, 0xCE, 0xF2, 0x0F, 0x11, 0x04, 0x24, 0xFF, 0xD2, 0xF3, 0x0F, 0x10, 0x40, 0x00, 0x0F, 0x5A, 0xC0};
	
	char folder[128];
	GetPrivateProfileString("Hack", "MapsFolder", "C:\\Maps\\", folder, 128, ".\\pedro.ini");
	char mapname[64];
	ReadProcessMemory(hack->process, hack->engine_pointer+m_dwMapName, mapname, sizeof(char)*64, 0);
	
	initscr();
	start_color();
	init_pair(1, COLOR_RED, COLOR_WHITE);
	init_pair(2, COLOR_GREEN, COLOR_WHITE);
	init_pair(3, COLOR_CYAN, COLOR_WHITE);
	
	hack->hackstatus=newwin(12, 20, 1, 1);
	hack->hackinfo=newwin(12, 50, 1, 22);
	box(hack->hackstatus, 0, 0);
	box(hack->hackinfo, 0, 0);
	
	touchwin(hack->hackstatus);
	touchwin(hack->hackinfo);
	
	noecho();
	curs_set(0);
	
	while (TRUE)
	{
		// if (GetAsyncKeyState(VK_ESCAPE)) break;
		
		bool ingame = (read_int(hack->process, hack->engine_pointer+m_dwInGame) == 6);
		
		if (ingame) {
			char mapdir[64];
			char mapname[64];
			
			ReadProcessMemory(hack->process, hack->engine_pointer+m_dwMapDirectory, mapdir, sizeof(char)*64, 0);
			ReadProcessMemory(hack->process, hack->engine_pointer+m_dwMapName, mapname, sizeof(char)*64, 0);
			
			bool parsed = hack->parser.parse_map(std::string(folder), std::string(mapname)+std::string(".bsp"));
			mvwprintw(hack->hackinfo, 1, 2, "Parsed Map File: %s", parsed ? "true" : "false");
			
			wattron(hack->hackstatus, COLOR_PAIR(2));
			mvwprintw(hack->hackstatus, 0, 1, "%-11s", "IN GAME");
		} else {
			wattron(hack->hackstatus, COLOR_PAIR(1));
			mvwprintw(hack->hackstatus, 0, 1, "%-11s", "NOT IN GAME");
		}
		// mvwprintw(hack->hackinfo, 1, 20, "%08x", local_player);
		mvwprintw(hack->hackinfo, 0, 1, "DEBUG");
		
		hack->local_player = read_int(hack->process, hack->client_dll+m_dwLocalPlayer);
		hack->player_count = 0;
		for (int i = 1; i < 64; i++)
		{
			int ent = read_int (hack->process, hack->client_dll+m_dwEntityList+(i-1)*16);

			int tn = read_int (hack->process, hack->entities[i].entity+m_iTeamNum);

			
			if (ent > 0) {
				hack->entities[hack->player_count].entity  = ent;//read_int (hack->process, hack->client_dll+m_dwEntityList+(i-1)*16);
				hack->entities[hack->player_count].dormant = read_bool(hack->process, ent+m_bDormant);
				hack->entities[hack->player_count].spotted = read_bool(hack->process, ent+m_bSpotted);
				hack->entities[hack->player_count].index   = read_int (hack->process, ent+m_iGlowIndex);
				hack->entities[hack->player_count].team    = read_int (hack->process, ent+m_iTeamNum);
				hack->entities[hack->player_count].health  = read_int (hack->process, ent+m_iHealth);
				hack->player_count++;
			}
		}
		
		MSG msg;
		int d = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		for (int i=0;i<hack->num_hacks;i++) {
			if (d != 0) {
				if (msg.lParam == hack->hack_array[i].hotkey*0x010000) hack->hack_array[i].enabled = !hack->hack_array[i].enabled;
			}
			
			char str[16];
			sprintf(str, "%-13s%-3s", hack->hack_array[i]._name, keys[(int)(hack->hack_array[i].hotkey) - 112]);
			wattron(hack->hackstatus, hack->hack_array[i].enabled ? COLOR_PAIR(2) : COLOR_PAIR(1));
			mvwprintw(hack->hackstatus, 2+i, 2, "%s", str);
			
			if (hack->hack_array[i].ex) hack->hack_array[i].ptr_ex(hack, hack->hack_array[i].enabled);
			if (hack->hack_array[i].enabled && !hack->hack_array[i].ex) hack->hack_array[i].ptr(hack);
		}
		
		wattron(hack->hackinfo, COLOR_PAIR(3));
		mvwprintw(hack->hackinfo, 10, 2, "SHA1: %s", hack->sha1);
		wattroff(hack->hackinfo, COLOR_PAIR(3));
		
		refresh();
		wrefresh(hack->hackstatus);
		wrefresh(hack->hackinfo);
		Sleep(1);
	}
}

void stop_hack(Hack* hack)
{
	delwin(hack->hackstatus);
	delwin(hack->hackinfo);
	endwin();
	
	for (int i=0;i<hack->num_hacks;i++) {
		pthread_cancel(hack->hack_array[i].thread);
	}
}

static DWORD find_dll(HANDLE process, const char* n)
{
	// this is some stupid shit, don't worry about it
	// this adds a backslash to the beginning of the name (eg \client.dll) to ensure
	// we get the actual client/engine.dll instead of something like steamclient.dll or whatever.
	char _name[32];
	strcpy(_name, "\\");
	strcpy(_name, n);
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	
	HMODULE mods[1024];
	DWORD cb;
	
	HMODULE mod;
	DWORD dll = NULL;
	
	if (EnumProcessModulesEx(process, mods, sizeof(mods), &cb, LIST_MODULES_ALL)) {
		for (int i = 0; i < (cb / sizeof(HMODULE)); i++) {
			TCHAR name[MAX_PATH];
			if (GetModuleFileNameEx(process, mods[i], name, sizeof(name) / sizeof(TCHAR))) {
				if (strstr(name, _name)) {
					mod = mods[i];
	}	}	}	}
	
	MODULEINFO info = {0};
	GetModuleInformation(process, mod, &info, sizeof(info));
	dll = (DWORD)info.lpBaseOfDll;
	
	return dll;
}

static void get_sha1(Hack* hack)
{
	// read buffer
	FILE* f = fopen("Pedro2.exe", "rb");
	if (f == NULL) return;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	char* string = malloc(fsize+1);
	fread(string, fsize, 1, f);
	fclose(f);
	
	// calc sha1
	SHA1_CTX sha;
	uint8_t results[20];
	SHA1Init(&sha);
	SHA1Update(&sha, (uint8_t*)string, fsize);
	SHA1Final(results, &sha);
	char buf[128];
	for (int i=0;i<20;i++) sprintf(buf, "%s%02x", buf, results[i]);
	hack->sha1 = malloc(sizeof(char) * 128);
	memcpy(hack->sha1, buf, sizeof(buf));
	
	for (int i=0;i<strlen(hack->sha1);i++) hack->sha1[i] = toupper(hack->sha1[i]);
	
}

static void thread_loop(ptr_t args)
{
	while (true) {
		if (args.hack->hack_array[args.id].enabled)
			args.ptr(args.hack);
		Sleep(1);
		// std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
