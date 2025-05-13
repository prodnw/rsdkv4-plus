#include "RetroEngine.hpp"

#if RETRO_USE_DISCORD_SDK
#include "discord.h"
discord::Core *__discord = {};

void API_Discord_Init()
{
    PrintLog("Initializing Discord API");
    //
}

discord::Core *API_Discord_GetCore()
{
    if (__discord)
        return __discord;

    return nullptr;
}
#endif

#if RETRO_USE_STEAMWORKS
#include "steam/steam_api.h"

bool API_Steam_hasPlusDLC = false;

void API_Steam_Init()
{
    PrintLog("Initializing Steam API");

    SteamErrMsg error = {};

    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid)) {
        // running = false;
    }

    if (SteamAPI_InitEx(&error) != k_ESteamAPIInitResult_OK) {
        PrintLog("Failed to initialize Steam API.  %s", error);
        return;
    }

    API_Steam_hasPlusDLC = SteamApps()->BIsDlcInstalled(API_STEAM_SONIC_ORIGINS_PLUS_DLC_ID);

    if (API_Steam_hasPlusDLC)
        PrintLog("[API TEST] API_Steam_hasPlusDLC is enabled");
    else
        PrintLog("[API TEST] API_Steam_hasPlusDLC is disabled");
}
#endif

void API_Init()
{
#if RETRO_USE_DISCORD_SDK
    API_Discord_Init();
#endif

#if RETRO_USE_STEAMWORKS
    API_Steam_Init();
#endif
}