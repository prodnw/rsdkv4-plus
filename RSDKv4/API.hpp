#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if RETRO_USE_DISCORD_SDK
#include "discord.h"
#define API_DISCORD_CLIENT_ID (1375887146057076747)

extern discord::Core *__discord;
extern discord::Activity __activity;
extern discord::ActivityAssets __assets;

enum API_Discord_PresenceTypes {
    PRESENCE_ACTIVITY_DETAILS,
    PRESENCE_ACTIVITY_STATE,
    PRESENCE_ASSET_LARGEIMAGE,
    PRESENCE_ASSET_LARGETEXT,
    PRESENCE_ASSET_SMALLIMAGE,
    PRESENCE_ASSET_SMALLTEXT,
};

void API_Discord_Init();
void API_Discord_Update();

void API_Discord_SetPresence(const char *text, int type);
void API_Discord_UpdatePresence();
#endif

#if RETRO_USE_STEAMWORKS
#include "steam/steam_api.h"
#define API_STEAM_SONIC_ORIGINS_PLUS_DLC_ID (2343200)

extern bool API_Steam_hasPlusDLC;

void API_Steam_Init();
#endif

void API_Init();
