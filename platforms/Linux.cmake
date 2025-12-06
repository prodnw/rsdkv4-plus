# PKG-CONFIG IS USED AS THE MAIN DRIVER
# bc cmake is inconsistent as fuuuckkk

find_package(PkgConfig REQUIRED)

add_executable(RetroEngine ${RETRO_FILES})

pkg_check_modules(OGG ogg)

if(NOT OGG_FOUND)
    set(COMPILE_OGG TRUE)
    message(NOTICE "libogg not found, attempting to build from source")
else()
    message("found libogg")
    target_link_libraries(RetroEngine ${OGG_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${OGG_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${OGG_STATIC_CFLAGS})
endif()

pkg_check_modules(THEORA theora theoradec)

if(NOT THEORA_FOUND)
    message("could not find libtheora, attempting to build manually")
    set(COMPILE_THEORA TRUE)
else()
    message("found libtheora")
    target_link_libraries(RetroEngine ${THEORA_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${THEORA_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${THEORA_STATIC_CFLAGS})
endif()

pkg_check_modules(VORBIS vorbis vorbisfile) #idk what the names are

if(NOT VORBIS_FOUND)
    set(COMPILE_VORBIS TRUE)
    message(NOTICE "libvorbis not found, attempting to build from source")
else()
    message("found libvorbis")
    target_link_libraries(RetroEngine ${VORBIS_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${VORBIS_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${VORBIS_STATIC_CFLAGS})
endif()

if(RETRO_USE_HW_RENDER)
    pkg_check_modules(GLEW glew)

    if(NOT GLEW_FOUND)
        message(NOTICE "could not find glew, attempting to build from source")

    else()
        message("found GLEW")
        target_link_libraries(RetroEngine ${GLEW_STATIC_LIBRARIES})
        target_link_options(RetroEngine PRIVATE ${GLEW_STATIC_LDLIBS_OTHER})
        target_compile_options(RetroEngine PRIVATE ${GLEW_STATIC_CFLAGS})
    endif()
endif()

if(RETRO_SDL_VERSION STREQUAL "2")
    pkg_check_modules(SDL2 sdl2 REQUIRED)
    target_link_libraries(RetroEngine ${SDL2_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${SDL2_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${SDL2_STATIC_CFLAGS})
elseif(RETRO_SDL_VERSION STREQUAL "1")
    pkg_check_modules(SDL1 sdl1 REQUIRED)
    target_link_libraries(RetroEngine ${SDL1_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${SDL1_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${SDL1_STATIC_CFLAGS})
endif()

if(RETRO_MOD_LOADER)
    set_target_properties(RetroEngine PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endif()

if(RETRO_USE_STEAM)
	if(RETRO_ARCH STREQUAL "64")
		set(STEAMWORKS_REDIST_BIN "${STEAMWORKS_SDK_DIR}/redistributable_bin/linux64")
		
		find_library(STEAM_API_LIB
			NAMES steam_api
			PATHS "${STEAMWORKS_REDIST_BIN}"

			NO_DEFAULT_PATH
		)
	elseif(RETRO_ARCH STREQUAL "32")
		set(STEAMWORKS_REDIST_BIN "${STEAMWORKS_SDK_DIR}/redistributable_bin/linux32")
		
		find_library(STEAM_API_LIB
			NAMES steam_api
			PATHS "${STEAMWORKS_REDIST_BIN}"

			NO_DEFAULT_PATH
		)
	endif()

	if(NOT STEAM_API_LIB)
		message(FATAL_ERROR "Steam API library not found in ${STEAMWORKS_REDIST_BIN}")
	else()
		message("found Steam API")
	endif()

	target_link_libraries(RetroEngine ${STEAM_API_LIB})
endif()

# TODO: borrowing this from S2M
if(RETRO_USE_DISCORD)
	set(CMAKE_STATIC_LIBRARY_PREFIX "")

	if(RETRO_ARCH STREQUAL "64")
		set(DISCORD_REDIST_BIN "${DISCORD_SDK_DIR}/lib/x86_64")
	elseif(RETRO_ARCH STREQUAL "32") # There are no 32 bit linux libraries yet-
										# (although the SDK got archived so I doubt there ever will be)
		message(WARNING "Discord SDK libraries for 32bit Linux may not exist")
		set(DISCORD_REDIST_BIN "${DISCORD_SDK_DIR}/lib/x86")
	endif()
	
# TODO: for some reason you gotta manually add the "lib" prefix to "libdiscord_game_sdk.so"
#		might be able to use "set_target_properties(discor PROPERTIES PREFIX "")", "CMAKE_STATIC_LIBRARY_PREFIX", or "CMAKE_SHARED_LIBRARY_PREFIX"
#		but for now ill just make it do it automatically
	if(EXISTS "${DISCORD_REDIST_BIN}/libdiscord_game_sdk.so")
		message("Discord Game SDK already has lib prefix")
	else()
		if(EXISTS "${DISCORD_REDIST_BIN}/discord_game_sdk.so")
			configure_file(
			  "${DISCORD_REDIST_BIN}/discord_game_sdk.so"
			  "${DISCORD_REDIST_BIN}/libdiscord_game_sdk.so"
			  COPYONLY
			)
		else()
			message(FATAL_ERROR "Discord Game SDK library not found in ${DISCORD_REDIST_BIN}")
		endif()
	endif()
	
	target_link_libraries(RetroEngine "${DISCORD_REDIST_BIN}/libdiscord_game_sdk.so")
endif()