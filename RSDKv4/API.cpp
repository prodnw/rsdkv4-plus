#include "RetroEngine.hpp"
#include "Userdata.hpp"

// Discord
#if RETRO_USE_DISCORD_SDK
#include "discord.h"
discord::Core *__discord = {};
discord::Activity __activity = {};
discord::ActivityAssets __assets = {};
uint64_t API_DISCORD_CLIENT_ID = 1375887146057076747; // Default to v4+ ID

void API_Discord_Init()
{
    PrintLog("Initializing Discord API");
    API_Discord_SetAppID();
    discord::Core::Create(API_DISCORD_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &__discord);
}

void API_Discord_SetAppID() {
#if RETRO_USE_MOD_LOADER
    if (discordGameClientID[0]) {
        API_DISCORD_CLIENT_ID = std::stoull(discordGameClientID);
        return;
    }
#endif
    
    switch(Engine.gameType) {
        case GAME_SONIC1:
            API_DISCORD_CLIENT_ID = 1375908654053593238; // Sonic 1 ID
            break;
        case GAME_SONIC2:
            API_DISCORD_CLIENT_ID = 1375909546874114209; // Sonic 2 ID
            break;
        case GAME_SONIC3:
            API_DISCORD_CLIENT_ID = 1375912145161683024; // Sonic 3 ID
            break;
        case GAME_SONICCD:
            API_DISCORD_CLIENT_ID = 1375916146154410145; // Sonic CD ID
            break;
        case GAME_SONICNEXUS:
            API_DISCORD_CLIENT_ID = 1375918952357826662; // Sonic Nexus ID
            break;
        case GAME_SONICDUELOFFATES:
            API_DISCORD_CLIENT_ID = 1375919192959746128; // Duel of Fates ID
            break;
        case GAME_SONICESSENCE:
            API_DISCORD_CLIENT_ID = 1383571039681777765; // Essence ID
            break;
        case GAME_PROJECT_SAP:
            API_DISCORD_CLIENT_ID = 1453058404892283130; // Project SAP ID
            break;
        case GAME_SONIC1FOREVER:
            API_DISCORD_CLIENT_ID = 1465393302542225520; // Sonic 1 Forever ID
            break;
        case GAME_SONIC2ABSOLUTE:
            API_DISCORD_CLIENT_ID = 1465394176609878254; // Sonic 2 Absolute ID
            break;
        case GAME_SONICCDINFINITE:
            API_DISCORD_CLIENT_ID = 1465394543561277717; // Sonic CD Infinite ID
            break;
        case GAME_ANATC:
            API_DISCORD_CLIENT_ID = 1465394350929477682; // A Night at the Casino ID
            break;
        default:
            API_DISCORD_CLIENT_ID = 1375887146057076747; // Default to V4+ ID
            break;
    }
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

void API_Discord_ClearPresenceType(int type)
{
    if (!__discord) return;

    switch (type) {
        case PRESENCE_ACTIVITY_DETAILS: __activity.SetDetails(""); break;
        case PRESENCE_ACTIVITY_STATE:   __activity.SetState("");   break;
        case PRESENCE_ASSET_LARGEIMAGE: __assets.SetLargeImage(""); break;
        case PRESENCE_ASSET_LARGETEXT:  __assets.SetLargeText("");  break;
        case PRESENCE_ASSET_SMALLIMAGE: __assets.SetSmallImage(""); break;
        case PRESENCE_ASSET_SMALLTEXT:  __assets.SetSmallText("");  break;

        default: break;
    }
}

void API_Discord_ClearAllPresence()
{
    if (!__discord) return;

    __activity.SetDetails("");
    __activity.SetState("");
    __assets.SetLargeImage("");
    __assets.SetLargeText("");
    __assets.SetSmallImage("");
    __assets.SetSmallText("");

    // push update to Discord immediately
    __activity.GetAssets()     = __assets;
    __discord->ActivityManager().UpdateActivity(__activity, [](discord::Result) {});
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

// Steam
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

// Epic Online Services (this seems like a HEAVY long shot but it would be so awesome to have in the engine)
#if RETRO_USE_EOS_SDK
#include "eos_sdk.h"

void API_EOS_Init()
{

}

void API_EOS_Update()
{

}
#endif

void API_Init()
{
#if RETRO_USE_DISCORD_SDK
    extern bool useDiscordRPC;
    if (useDiscordRPC)
        API_Discord_Init();
#endif

#if RETRO_USE_STEAMWORKS
    API_Steam_Init();
#endif

#if RETRO_USE_EOS_SDK
    API_EOS_Init();
#endif
}
