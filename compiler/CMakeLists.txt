project(equelle_compiler)
cmake_minimum_required( VERSION 2.8 )

# The -Wno-sign-compare is because flex generates a scanner with
# signed/unsigned comparison!
# Similarly, -Wno-deprecated-register is used because the deprecated
# 'register' storage class is used in the generated scanner.

if(NOT MSVC)
	set( CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-deprecated-register -Wno-unneeded-internal-declaration" )
endif()

find_package( BISON REQUIRED )
find_package( FLEX REQUIRED )
find_package( Boost REQUIRED COMPONENTS program_options )

bison_target( MyParser equelle_parser.y ${CMAKE_CURRENT_BINARY_DIR}/equelle_parser.cpp)
flex_target(MyScanner equelle_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/equelle_lexer.cpp )
add_flex_bison_dependency(MyScanner MyParser)

include_directories( . ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB EC_SOURCES "*.cpp" )
file( GLOB to_remove el.cpp )
list( REMOVE_ITEM EC_SOURCES ${to_remove} )

file( GLOB EC_HEADERS "*.hpp" )

add_executable( ec ${EC_SOURCES} ${EC_HEADERS} ${FLEX_MyScanner_OUTPUTS} 
${BISON_MyParser_OUTPUTS} )
set_target_properties( ec PROPERTIES COMPILE_DEFINITIONS "RETURN_TOKENS=1" )
target_link_libraries(ec ${Boost_LIBRARIES})

add_executable( el el.cpp equelle_parser.y equelle_lexer.l ${FLEX_MyScanner_OUTPUTS} )

install(TARGETS ec el 
	EXPORT EquelleTargets
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin )





