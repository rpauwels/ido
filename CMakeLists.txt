cmake_minimum_required(VERSION 3.8.0)
add_library(wt SHARED IMPORTED)
#target_link_libraries(wt wthttp)
add_executable(rsvpApplication rsvpApplication.cpp party.cpp guest.cpp)
