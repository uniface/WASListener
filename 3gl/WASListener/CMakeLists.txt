# CMakeList.txt : CMake project for WASListener, include source and define
# project specific logic here.
#
cmake_minimum_required ( VERSION 3.8 )

set ( SOURCE_FILES 
	"CommandLine.cpp" 
	"FileAction.cpp" 
	"FolderWatcher.cpp" 
	"Notifications.cpp" 
	"Uniface.cpp" 
	"WASListener.cpp" 
)

set ( HEADER_FILES 
  "CommandLine.h" 
  "FileAction.h"
  "FolderWatcher.h"
  "Notifications.h"
  "resource.h"
  "targetver.h"
  "Uniface.h"
  "WASListener.h"
)

set ( EXTRA_FILES 
	"WASListener.exe.manifest"
	"WASListener.rc"
)

set (UNIFACE_LIBRARIES 
	"ucall"
)

set( UNIFACE_3GL_FOLDER "" CACHE PATH "The Uniface root install folder" )
message( STATUS "${UNIFACE_3GL_FOLDER}" )

set( Boost_USE_STATIC_LIBS ON )
find_package( Boost REQUIRED COMPONENTS program_options )
if( Boost_FOUND )
	add_compile_options( /W4 /WX )
	add_definitions( -D_UNICODE -DUNICODE )

	include_directories( ${Boost_INCLUDE_DIRS} ${UNIFACE_3GL_FOLDER}/include )
	link_directories(${Boost_LIBRARY_DIRS} ${UNIFACE_3GL_FOLDER}/lib )

	add_executable ( WASListener WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${EXTRA_FILES} )
	target_link_libraries( WASListener ${UNIFACE_LIBRARIES} ${Boost_LIBRARIES} )

	set( CMAKE_CXX_STANDARD_REQUIRED ON )
	set_property( TARGET WASListener PROPERTY CXX_STANDARD 17 )
endif()

