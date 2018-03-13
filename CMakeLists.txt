cmake_minimum_required(VERSION 3.9)

project("Stage Figther")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(BINARY "Stage_Figther")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(EXTERN "${CMAKE_SOURCE_DIR}/extern")

set(CMAKE_CXX_STANDARD 11)

#
# Comiler Stuff:
#

if (MSVC)
	set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)

	foreach (flag CMAKE_C_FLAGS
				  CMAKE_C_FLAGS_DEBUG
				  CMAKE_C_FLAGS_RELEASE
				  CMAKE_C_FLAGS_MINSIZEREL
				  CMAKE_C_FLAGS_RELWITHDEBINFO)

		if (${flag} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
		endif()
		if (${flag} MATCHES "/MDd")
			string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
		endif()

	endforeach()
endif()

#
# Dependencies:
#

## GLFW ###
if(UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW REQUIRED glfw3)
else()
	if(NOT EXISTS "${EXTERN}/glfw/CMakeLists.txt")
		find_package(glfw REQUIRED)
	endif()
endif()

if(NOT GLFW_FOUND)
	message(STATUS "Using GLFW in ${EXTERN}/glfw")
	
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

	add_subdirectory("${EXTERN}/glfw")
	
	set(GLFW_INCLUDE_DIRS "${EXTERN}/glfw/include")
	set(GLFW_STATIC_LIBRARIES "glfw")
endif()

## BULLET ###
find_package(Bullet CONFIGS "BulletConfig.cmake")
if(BULLET_FOUND)
	if(UNIX)
		include_directories("/usr/include/bullet")
	endif()
else()
	include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
	message(STATUS "Using Bullet in ${EXTERN}/bullet3")
	  
	 list(APPEND BulletCmakeArgs
		-DCMAKE_INSTALL_PREFIX=${EXTERN}/bullet3
        -DBUILD_BULLET2_DEMOS=OFF 
        -DBUILD_CLSOCKET=OFF 
        -DBUILD_CPU_DEMOS=OFF 
        -DBUILD_ENET=OFF 
        -DBUILD_OPENGL3_DEMOS=OFF 
        -DBUILD_UNIT_TESTS=OFF 
        -DINCLUDE_INSTALL_DIR=include/ 
        -DLIB_DESTINATION=lib/
        -DUSE_GRAPHICAL_BENCHMARK=OFF 
        -DBULLET2_USE_OPEN_MP_MULTITHREADING=ON 
        -DINSTALL_LIBS=ON 
		-DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -G ${CMAKE_GENERATOR}
      )
	  
	  ExternalProject_Add(Bullet 
		PREFIX "${EXTERN}/bullet3"
		BUILD_IN_SOURCE 1
		SOURCE_DIR "${EXTERN}/bullet3"
		CMAKE_ARGS "${BulletCmakeArgs}"
		LOG_BUILD 1
	)

	set (BULLET_LIBS_RAW
		LinearMath
		Bullet3Common
		BulletInverseDynamics
		BulletCollision
		BulletDynamics
		BulletSoftBody
	)
	
	if(MSVC)
		set (BULLET_LIBRARIES "")
		foreach(x ${BULLET_LIBS_RAW})
			list (APPEND BULLET_LIBRARIES debug ${x}_Debug optimized ${x})
		endforeach()
	else()
		set(BULLET_LIBRARIES ${BULLET_LIBS_RAW})
	endif()

	set(BULLET_INCLUDE_DIRS "${EXTERN}/bullet3/include")
	
endif()

## GLAD ##
set(GLAD_INCLUDE_DIR "${EXTERN}/glad/include")
add_library("glad" "${EXTERN}/glad/src/glad.c" src/object3d/Triangle.cpp src/object3d/Triangle.h)
target_include_directories("glad" PRIVATE "${GLAD_INCLUDE_DIR}")

## GLM ##
find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})

## Kaguya ##
if (UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_search_module(lua REQUIRED lua5.3)

	if(LUA_FOUND)
		message(STATUS "Lua was found by pkg-config: '${LUA_INCLUDE_DIRS}'")
		if(LUA_INCLUDE_DIRS STREQUAL "")
			message(STATUS "Assume that lua.h is in /usr/include")
			set(LUA_INCLUDE_DIRS "/usr/include")
		endif()
	endif()
endif()

if(NOT LUA_FOUND)
	set(LOCAL_LUA_DIRECTORY "${EXTERN}/lua")
endif()

set(KAGUYA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(KAGUYA_BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
set(KAGUYA_BUILD_EXAMPLE_GLFW_BIND OFF CACHE BOOL "" FORCE)
set(KAGUYA_SINGLE_HEADER_VERSION OFF CACHE BOOL "" FORCE)

add_subdirectory("${EXTERN}/kaguya")

set(KAGUYA_INCLUDE_DIRS "${EXTERN}/kaguya/include")

## Logger ##
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory("${EXTERN}/spdlog")
include_directories("${EXTERN}/spdlog/include")

## stb single headers ##
include_directories("${EXTERN}/stb")

#
# Link & Include Stuff into the binary
#

include_directories(
		${GLFW_INCLUDE_DIRS}
		${BULLET_INCLUDE_DIRS}
		${KAGUYA_INCLUDE_DIRS}
        PRIVATE ${GLAD_INCLUDE_DIR}
	)

add_executable(
		"${BINARY}"
		src/main.cpp
		src/Window.cpp src/Window.h src/helper/Callback.h src/BulletUniverse.cpp src/BulletUniverse.h src/object3d/Object3D.cpp src/object3d/Object3D.h src/Shader.cpp src/Shader.h src/manager/ShaderManager.cpp src/manager/ShaderManager.h src/Texture.cpp src/Texture.h)

set_target_properties("${BINARY}" PROPERTIES LINKER_LANGUAGE CXX)	
	
target_link_libraries(
		"${BINARY}"
		${GLFW_STATIC_LIBRARIES}
		${BULLET_LIBRARIES}
		${LUA_LIBRARIES}
		glad
        Threads::Threads
	)

if(NOT BULLET_FOUND)	
	add_dependencies(${BINARY} Bullet)	
endif()