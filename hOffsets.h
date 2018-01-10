#ifndef HOFFSETS_H
#define HOFFSETS_H

#include <iostream>
#include <Windows.h>

const DWORD dw_playerbase = 0x53BFC8; // Player Data Structure // DWORD = 4 bytes long aka 32 bits
const DWORD dw_entitybase = 0x5495B4; // Entity Base (list of players, which in this case includes you)
const DWORD dw_entity_loop_distance = 0x10; // The distance between each player data structure
const DWORD dw_mouse_x = 0x457874; // Engine.dll X, Y coordinates ingame... SetCursor 
const DWORD dw_numofplayers = 0x5C817C; // How many players are in the game

const DWORD dw_health_offset = 0x90; // Health
const DWORD dw_team_offset = 0x98; // Team 1 - Spec, 2 - T, 3 - CT
const DWORD dw_dormant_offset = 0x17A; // Updated? 
const DWORD dw_position_offset = 0x25C; // Position
const DWORD dw_flags_offset = 0x34C; // Flags? 
const DWORD dw_bone_offset = 0x570; // Bone Offset
const DWORD dw_fov_offset = 0xFBC; // FOV
const DWORD dw_activeweapon = 0xD70; // Active
const DWORD dw_weaponid = 0x594; // Weapon ID
const DWORD dw_crosshair_offset = 0x14D1; // We can use this as a triggerbot

const DWORD dw_vmatrix = 0x58645C; // View Matrix
const DWORD dw_antiFlick = dw_vmatrix + 0x4C; // Flicker Check // 0x4C == 1 // OLD ADDRESS -0x1A4;
const DWORD dw_antiFlick2 = dw_vmatrix + 0x54; // 0x54 == 0 
const DWORD dw_antiFlick3 = dw_vmatrix + 0x7C; // 0x7C == 0 
const DWORD dw_antiFlick4 = dw_vmatrix + 0x84; // 0x84 == 0 

#endif