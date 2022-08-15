cmake_minimum_required(VERSION 3.15)

set(LUA_SOURCE  lapi.c
                lauxlib.c
                lbaselib.c
                lcode.c
                lcorolib.c
                lctype.c
                ldblib.c
                ldebug.c
                ldo.c
                ldump.c
                lfunc.c
                lgc.c
                linit.c
                liolib.c
                llex.c
                lmathlib.c
                lmem.c
                loadlib.c
                lobject.c
                lopcodes.c
                loslib.c
                lparser.c
                lstate.c
                lstring.c
                lstrlib.c
                ltable.c
                ltablib.c
                ltm.c
                #lua.c
                #luac.c
                lundump.c
                lutf8lib.c
                lvm.c
                lzio.c)

set(LUA_HEADER  lapi.h
                lauxlib.h
                lcode.h
                lctype.h
                ldebug.h
                ldo.h
                lfunc.h
                lgc.h
                ljumptab.h
                llex.h
                llimits.h
                lmem.h
                lobject.h
                lopcodes.h
                lopnames.h
                lparser.h
                lprefix.h
                lstate.h
                lstring.h
                ltable.h
                ltm.h
                lua.h
                lua.hpp
                luaconf.h
                lualib.h
                lundump.h
                lvm.h
                lzio.h)

add_library(Lua STATIC ${LUA_SOURCE} ${LUA_HEADER})
target_include_directories(Lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if (UNIX)
target_compile_options(Lua PRIVATE -fPIC)
endif()
