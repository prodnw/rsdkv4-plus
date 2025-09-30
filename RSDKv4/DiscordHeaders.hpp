#ifndef DISCORDHEADERS_H
#define DISCORDHEADERS_H
#if RETRO_USE_DISCORD_SDK

// cstdint is not in C
// and cmake compiles this in both C and C++ for some reason, so...
#ifdef __cplusplus
#include <cstdint>
#endif //! __cplusplus

#endif //! RETRO_USE_DISCORD_SDK
#endif //! DISCORDHEADERS_H