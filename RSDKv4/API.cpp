#include "RetroEngine.hpp"

#if RETRO_USE_DISCORD_SDK
#include "cpp/discord.h"
discord::Core *__discord = {};

void API_Discord_Init()
{
    PrintLog("Initializing Discord API");

    discord::Core::Create(API_DISCORD_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &__discord);
}

void API_Discord_Update() // used in RetroGameLoop_Main
{
    if (__discord)
        API_Discord_GetCore()->RunCallbacks();
}

discord::Core *API_Discord_GetCore()
{
    if (__discord)
        return __discord;

    return nullptr;
}

void API_Discord_SetRPCStatus(const char *details, const char *state, const char *largeImage, const char *largeText, const char *smallImage, const char *smallText)
{
    if (__discord) {
        discord::Activity activity = {};
        activity.SetDetails(details);
        activity.SetState(state);

        discord::ActivityAssets activityAssets = {};
        activityAssets.SetLargeImage(largeImage);
        activityAssets.SetLargeText(largeText);
        activityAssets.SetSmallImage(smallImage);
        activityAssets.SetSmallText(smallText);

        discord::ActivityTimestamps activityTimestamps = {};
        activityTimestamps.SetStart(time(nullptr));
        activity.GetTimestamps() = activityTimestamps;
        activity.GetAssets()     = activityAssets;

        __discord->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
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
