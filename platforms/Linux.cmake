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

	# TODO this if() kinda stinky,,, but i cant figure out any other way to do it
	if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
		set(STEAMWORKS_REDIST_BIN "${STEAMWORKS_SDK_DIR}/redistributable_bin/linux64")
		message("System is supected to be 64 bit")
	else()
		set(STEAMWORKS_REDIST_BIN "${STEAMWORKS_SDK_DIR}/redistributable_bin/linux32")
		message("System is supected to be 32 bit")
	endif()

	find_library(STEAM_API_LIB
		NAMES libsteam_api.so
		PATHS "${STEAMWORKS_REDIST_BIN}"

		NO_DEFAULT_PATH
	)

	# there should be an error for the above if this isnt found - but for now,

	target_link_libraries(RetroEngine ${STEAM_API_LIB})

endif()