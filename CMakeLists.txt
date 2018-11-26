project(ido)
cmake_minimum_required(VERSION 3.6.3)

find_package(Wt NO_DEFAULT_PATH REQUIRED)
set(CMAKE_CXX_STANDARD 14)
add_executable(${PROJECT_NAME}.wt 
	src/rsvpApplication.cpp
	src/adminApplication.cpp
	src/calendarResource.cpp
	src/event.cpp
	approot/resources.xml
	approot/wt_config.xml
)
target_link_libraries(${PROJECT_NAME} wt wtdbo wtdbosqlite3 wthttp)
install(TARGETS rsvpApplication DESTINATION /usr/local/bin)
