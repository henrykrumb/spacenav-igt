cmake_minimum_required(VERSION 3.26)
project(spacenavigt)
include(FetchContent)

FetchContent_Declare(
    OpenIGTLink
    GIT_REPOSITORY	https://github.com/openigtlink/OpenIGTLink
    GIT_TAG			c512727425c2b7a594fabb9cd1fbfac512bf376e
)
FetchContent_MakeAvailable(OpenIGTLink)

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        20de8b73bbbabaf2f94dd07c4ece8ff3590af531
)
FetchContent_MakeAvailable(cli11)

ExternalProject_Add(
    spnav
    GIT_REPOSITORY https://github.com/FreeSpacenav/libspnav
    GIT_TAG master
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/spnav-prefix/src/spnav && ./configure
    BUILD_COMMAND ${CMAKE_COMMAND} -E env
        CC=${CMAKE_C_COMPILER}
        make -C ${CMAKE_CURRENT_BINARY_DIR}/spnav-prefix/src/spnav
    INSTALL_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/spnav-prefix/src/spnav && make install PREFIX=${CMAKE_CURRENT_BINARY_DIR}/install
)
ExternalProject_Get_Property(spnav install_dir)
add_library(lspnav STATIC IMPORTED)
set_property(TARGET lspnav PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libspnav.so)
add_dependencies(lspnav spnav)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(X11 REQUIRED)
include_directories(${install_dir}/include ${X11_INCLUDE_DIR} ${cli11_SOURCE_DIR}/include)
link_directories(${X11_LIBRARIES})

add_executable(spnavigt src/utils.cpp src/spnavigt.cpp)
target_link_libraries(spnavigt OpenIGTLink ${X11_LIBRARIES} lspnav)
set_target_properties(spnavigt PROPERTIES
    OUTPUT_NAME spnavigt
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)


install(
	TARGETS spnavigt
	DESTINATION /usr/local/bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
