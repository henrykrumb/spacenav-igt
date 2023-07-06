project(spacenavigt)
include(FetchContent)

FetchContent_Declare(
    OpenIGTLink
    GIT_REPOSITORY	https://github.com/openigtlink/OpenIGTLink
    GIT_TAG			d4eaae937b6a28ea2324f90c128800b3ad1cf2b3
)
FetchContent_MakeAvailable(OpenIGTLink)

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
include_directories(${install_dir}/include)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

add_executable(spnavigt src/spnavigt.cpp)
target_link_libraries(spnavigt OpenIGTLink ${X11_LIBRARIES} lspnav)
set_target_properties(spnavigt PROPERTIES OUTPUT_NAME spnavigt)


install(
	TARGETS spnavigt
	DESTINATION /usr/local/bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)