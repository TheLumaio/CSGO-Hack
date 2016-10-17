#include <stdio.h>

#include "hack.h"
#include "util.h"

#define DEBUG true

struct mapplayer_t{
	float  AbsOrigin[3];     
	DWORD   unknown[ 10 ]; // to lazy to find out whats in there
	int     Health;            
	wchar_t Name[ 32 ];          
};

void radar_hack(Hack* hack)
{
	for (int i=0;i<hack->player_count;i++) {
		if (!hack->entities[i].spotted)
			write_bool(hack->process, hack->entities[i].entity + m_bSpotted, TRUE);
	}
}

void wall_hack(Hack* hack)
{
	// I have to parse entities manually for some dumbfuck reason volvo plz
	for (int i = 1; i < 64; i++) {
		int entity = read_int(hack->process, hack->client_dll+m_dwEntityList + (i-1)*16);
		bool dor = read_bool(hack->process, entity+m_bDormant);
		if (dor) continue;
		int team = read_int(hack->process, entity+m_iTeamNum);
		int index = read_int(hack->process, entity+m_iGlowIndex);
		float lpos[3];
		float fpos[3];
		read_vec3(hack->process, hack->local_player+m_vecOrigin, lpos);
		read_vec3(hack->process, entity+m_vecOrigin, fpos);
		lpos[2] += 64;
		
		int glowobj = read_int(hack->process, hack->client_dll+m_dwGlowObject);
		int local_team = read_int(hack->process, hack->local_player+m_iTeamNum);
		float rgba[4];
		bool glow[2] = {true,false};
		
		if (team != local_team) {
			bool visible = hack->parser.is_visible(Valve::Vector3{floor(lpos[0]), floor(lpos[1]), floor(lpos[2])}, Valve::Vector3{floor(fpos[0]), floor(fpos[1]), floor(fpos[2])});
			if (visible) {
				rgba[0] = 0.3f;
				rgba[1] = 1.0f;
				rgba[2] = 0.3f;
				rgba[3] = 0.7f;
			} else {
				rgba[0] = 1.0f;
				rgba[1] = 0.3f;
				rgba[2] = 0.3f;
				rgba[3] = 0.7f;
			}
		} else {
			rgba[0] = 0.3f;
			rgba[1] = 0.3f;
			rgba[2] = 1.0f;
			rgba[3] = 0.7f;
		}
		
		write_vec4(hack->process, glowobj+index*0x38+0x4, rgba);
		write_bool(hack->process, glowobj+index*0x38+0x24, true);
		write_bool(hack->process, glowobj+index*0x38+0x25, false);
	}
}

void bhop_hack(Hack* hack)
{
	SHORT keydown = GetAsyncKeyState(0x20);
	if (keydown&0x8000)
	{
		int flags = read_int(hack->process, hack->local_player+m_fFlags);
		bool in_air = flags == 256 || flags == 258 || flags == 260 || flags == 262;
		if (!in_air) {
			write_int(hack->process, hack->client_dll + m_dwForceJump, 5);
		} else if (in_air) {
			write_int(hack->process, hack->client_dll + m_dwForceJump, 4);
		} else {
			write_int(hack->process, hack->client_dll + m_dwForceJump, 4);
		}
	}
}

void noflash_hack(Hack* hack, bool enabled)
{
	if (enabled) write_float(hack->process, hack->local_player+m_flFlashMaxAlpha, 0.f);
	else  write_float(hack->process, hack->local_player+m_flFlashMaxAlpha, 255.f);
}

void aimbot_hack(Hack* hack)
{
	if (!GetAsyncKeyState(0x12)) /* Left ALT */ { return; }
	
	int local_team = read_int(hack->process, hack->local_player+m_iTeamNum);
	float lpos[3];
	read_vec3(hack->process, hack->local_player+m_vecOrigin, lpos);
	lpos[2] += 64;
	
	int closest = -1;
	float dist=10000000,TMP;
	for (int i = 0; i < hack->player_count; i++)
	{
		if (!hack->entities[i].entity) continue;
		
		DWORD bone_matrix = read_dword(hack->process, hack->entities[i].entity + m_dwBoneMatrix);
		float head[3];
		head[0] = read_float(hack->process, bone_matrix+6*0x30+0x0C);
		head[1] = read_float(hack->process, bone_matrix+6*0x30+0x1C);
		head[2] = read_float(hack->process, bone_matrix+6*0x30+0x2C);
		
		TMP = get3D(lpos[0], lpos[1], lpos[2], head[0], head[1], head[2]);
		
		if (hack->entities[i].health != 0 && hack->entities[i].team != local_team) {
			float matrix[4][4];
			read_mat4(hack->process, hack->client_dll+m_dwViewMatrix, matrix);
			bool immune = read_bool(hack->process, hack->entities[i].entity+m_bGunGameImmunity);
			if (TMP < dist && !immune && !hack->entities[i].dormant) {
				if (hack->parser.is_visible(Valve::Vector3{floor(lpos[0]), floor(lpos[1]), floor(lpos[2])}, Valve::Vector3{floor(head[0]), floor(head[1]), floor(head[2])})) {
					closest = i;
					dist = TMP;
				}
			}
		}
	}
	
	if (closest > -1)
	{
		DWORD bone_matrix = read_dword(hack->process, hack->entities[closest].entity + m_dwBoneMatrix);
		float head[3];
		head[0] = read_float(hack->process, bone_matrix+6*0x30+0x0C);
		head[1] = read_float(hack->process, bone_matrix+6*0x30+0x1C);
		head[2] = read_float(hack->process, bone_matrix+6*0x30+0x2C);
		
		int flags = read_int(hack->process, hack->local_player+m_fFlags);
		
		float angle[3];
		calcAngle(lpos, head, angle, flags);
		
		// auto punchAngles = *pLocal->AimPunch() * 2.0f;
		// newAngles -= punchAngles;
		
		float punch[3];
		read_vec3(hack->process, hack->local_player+m_vecPunch, punch);
		punch[0] *=2;
		punch[1] *=2;
		punch[2] *=2;
		
		angle[0] -= punch[0];
		angle[1] -= punch[1];
		angle[2] -= punch[2];

		
		if (angle[0] != angle[0] || angle[1] != angle[1] || angle[2] != angle[2]) goto BANGLE;
		if ((int)(angle[0]) == 0 && (int)(angle[1]) == 0 && (int)(angle[2]) == 0) goto BANGLE;
		
		normalizeAngle(angle);
		vclamp(angle);
		if (angle[0] > 89 || angle[0] < -89 || angle[1] > 180 || angle[1] < -180) {
			goto BANGLE;
		}
		write_vec3(hack->process, hack->engine_pointer+m_dwViewAngles, angle);
		
	}
	
	BANGLE:
	return;
}

void trigger_hack(Hack* hack) {
	if (!GetAsyncKeyState(0x04)) return;
	
	int crosshairid = read_int(hack->process, hack->local_player+m_iCrossHairID);
	int enemyincross = read_int(hack->process, hack->client_dll+m_dwEntityList+((crosshairid-1)*0x10));
	int ecrosshealth = read_int(hack->process, enemyincross+m_iHealth);
	int ecrossteam = read_int(hack->process, enemyincross+m_iTeamNum);
	int localteam = read_int(hack->process, hack->local_player+m_iTeamNum);
	
	if (ecrosshealth > 0 && crosshairid > 0 && ecrossteam != localteam) {
		write_int(hack->process, hack->client_dll+m_dwForceAttack, 5);
		Sleep(50);
		write_int(hack->process, hack->client_dll+m_dwForceAttack, 4);
	}
}

static float fin[3];
static float old[3];

void recoil_hack(Hack* hack) {
	float aim[3];
	read_vec3(hack->process, hack->engine_pointer+m_dwViewAngles, aim);
	int fired = read_int(hack->process, hack->local_player+m_iShotsFired);
	
	if (fired > 0) {
		float _punch[3];
		read_vec3(hack->process, hack->local_player+m_vecPunch, _punch);
		_punch[0] *= 2;
		_punch[1] *= 2;
		
		fin[0] = aim[0] + (old[0] - _punch[0]);
		fin[1] = aim[1] + (old[1] - _punch[1]);
		fin[2] = aim[2] + (old[2] - _punch[2]);
		
		normalizeAngle(fin);
		vclamp(fin);
		write_vec3(hack->process, hack->engine_pointer+m_dwViewAngles, fin);
		
		old[0] = _punch[0];
		old[1] = _punch[1];
		old[2] = _punch[2];
	} else {
		
		fin[0] = 0;
		fin[1] = 0;
		fin[2] = 0;
		old[0] = 0;
		old[1] = 0;
		old[2] = 0;
	}
}

int main(void)
{
	Hack hack;
	init_hack(&hack, "csgo.exe");
	
	if (hack.client_dll)     printf("found client.dll\n");
	if (hack.engine_dll)     printf("found engine.dll\n");
	if (hack.engine_pointer) printf("found engine_pointer\n");
	
	add_hack(&hack, recoil_hack, "RC System", 0x72);
	add_hack(&hack, radar_hack, "Radarhack", 0x73);
	add_hack(&hack, wall_hack , "Wallhack" , 0x74);
	add_hack(&hack, bhop_hack , "Bunnyhop" , 0x75);
	add_hack_ex(&hack, noflash_hack, "Noflash", 0x76);
	add_hack(&hack, aimbot_hack, "Aimbot", 0x77);
	add_hack(&hack, trigger_hack, "Triggerbot", 0x78);
	
	start_hack(&hack);
	stop_hack(&hack);
	
}
