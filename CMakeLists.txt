cmake_minimum_required(VERSION 3.8.0)
add_executable(rsvpApplication rsvpApplication.cpp adminApplication.cpp calendarResource.cpp event.cpp)
target_link_libraries(rsvpApplication wt wtdbo wtdbosqlite3 wthttp)
install(TARGETS rsvpApplication DESTINATION /usr/local/bin)
