#include "RetroEngine.hpp"

#if RETRO_USE_DISCORD_SDK
#include "discord.h"
discord::Core *__discord = {};
discord::Activity __activity = {};
discord::ActivityAssets __assets = {};

void API_Discord_Init()
{
    PrintLog("Initializing Discord API");

    discord::Core::Create(API_DISCORD_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &__discord);
}

void API_Discord_Update() // used in ProcessStage
{
    if (__discord)
        __discord->RunCallbacks();
}

void API_Discord_SetPresence(const char *text, int type)
{
    if (!__discord)
        return;

    switch (type) {
        case PRESENCE_ACTIVITY_DETAILS: __activity.SetDetails(text); break;
        case PRESENCE_ACTIVITY_STATE:   __activity.SetState(text);   break;

        case PRESENCE_ASSET_LARGEIMAGE: __assets.SetLargeImage(text); break;
        case PRESENCE_ASSET_LARGETEXT:  __assets.SetLargeText(text);  break;
        case PRESENCE_ASSET_SMALLIMAGE: __assets.SetSmallImage(text); break;
        case PRESENCE_ASSET_SMALLTEXT:  __assets.SetSmallText(text);  break;

        default: break;
    }
}

void API_Discord_UpdatePresence()
{
    if (__discord) {
        discord::ActivityTimestamps activityTimestamps = {};
        activityTimestamps.SetStart(time(nullptr));

        __activity.GetTimestamps() = activityTimestamps;
        __activity.GetAssets()     = __assets;

        __discord->ActivityManager().UpdateActivity(__activity, [](discord::Result result) {});
    }
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
