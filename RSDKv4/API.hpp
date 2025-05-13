#ifndef API_H
#define API_H

#if RETRO_USE_DISCORD_SDK
#include "discord.h"
extern discord::Core *__discord;

void API_Discord_Init();
discord::Core *API_Discord_GetCore();
#endif

#if RETRO_USE_STEAMWORKS
#include "steam/steam_api.h"
#define API_STEAM_SONIC_ORIGINS_PLUS_DLC_ID (2343200)

extern bool API_Steam_hasPlusDLC;

void API_Steam_Init();
#endif

void API_Init();

#endif