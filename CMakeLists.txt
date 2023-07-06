project(spacenavigt)
include(FetchContent)

FetchContent_Declare(
    OpenIGTLink
    GIT_REPOSITORY	https://github.com/openigtlink/OpenIGTLink
    GIT_TAG			d4eaae937b6a28ea2324f90c128800b3ad1cf2b3
)
FetchContent_MakeAvailable(OpenIGTLink)

FetchContent_Declare(
    spnav
    GIT_REPOSITORY https://github.com/FreeSpacenav/libspnav
    GIT_TAG master
    CONFIGURE_COMMAND "./configure"
    BUILD_COMMAND "make"
)
FetchContent_MakeAvailable(spnav)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
add_executable(spnavigt src/spnavigt.cpp)
target_link_libraries(spnavigt OpenIGTLink spnav)
set_target_properties(spnavigt PROPERTIES OUTPUT_NAME spnavigt)

install(
	TARGETS spnavigt
	DESTINATION /usr/local/bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)