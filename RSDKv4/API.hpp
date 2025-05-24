#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if RETRO_USE_DISCORD_SDK
#include "discord.h"
#define API_DISCORD_CLIENT_ID (0) // store this as an int, load externally

extern discord::Core *__discord;

void API_Discord_Init();
void API_Discord_Update();
discord::Core *API_Discord_GetCore();
void API_Discord_SetRPCStatus(const char *details, const char *state, const char *largeImage, const char *largeText, const char *smallImage, const char *smallText);
#endif

#if RETRO_USE_STEAMWORKS
#include "steam/steam_api.h"
#define API_STEAM_SONIC_ORIGINS_PLUS_DLC_ID (2343200)

extern bool API_Steam_hasPlusDLC;

void API_Steam_Init();
#endif

void API_Init();
