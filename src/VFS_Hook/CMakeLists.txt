﻿if (NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
	return()
endif()

# Value Define
set(PROJECT_NAME VFS_Hook)

# Cpp Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Library
link_libraries(Valkyria)

# Current Project
project("${PROJECT_NAME}")
add_library(${PROJECT_NAME} MODULE "dllmain.cpp")
