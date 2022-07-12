# CMakeList.txt : CMake project for WASListener, include source and define
# project specific logic here.
#
cmake_minimum_required ( VERSION 3.15 )

set ( SOURCE 
	"CommandLine.cpp" 
	"FileAction.cpp" 
	"FolderWatcher.cpp" 
	"Notifications.cpp" 
	"Uniface.cpp" 
	"WASListener.cpp" 
)

set ( HEADER 
  "CommandLine.h" 
  "FileAction.h"
  "FolderWatcher.h"
  "Notifications.h"
  "resource.h"
  "targetver.h"
  "Uniface.h"
  "WASListener.h"
)

set ( EXTRA
	"WASListener.exe.manifest"
	"WASListener.rc"
)

set( UNIFACE_3GL_FOLDER "" CACHE PATH "The Uniface root install folder" )
message( STATUS "${UNIFACE_3GL_FOLDER}" )

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost REQUIRED COMPONENTS program_options thread)

if( Boost_FOUND )
	add_compile_options( /W4 /WX )
	add_definitions( -D_UNICODE -DUNICODE )

	add_executable ( WASListener WIN32 ${SOURCE} ${HEADER} ${EXTRA} )
	target_include_directories( WASListener PRIVATE ${UNIFACE_3GL_FOLDER}/include )
	target_link_libraries( WASListener PRIVATE 
		${UNIFACE_3GL_FOLDER}/lib/ucall.lib 
		Boost::program_options
		Boost::thread
	)

	set( CMAKE_CXX_STANDARD_REQUIRED ON )
	set_property( TARGET WASListener PROPERTY CXX_STANDARD 17 )
endif()

