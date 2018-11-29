project(ido)
cmake_minimum_required(VERSION 3.6.3)

find_package(Wt NO_DEFAULT_PATH REQUIRED)
find_package(Doxygen)
set(CMAKE_CXX_STANDARD 14)
add_executable(${PROJECT_NAME}.wt 
	src/rsvpApplication.cpp
	src/adminApplication.cpp
	src/calendarResource.cpp
	src/event.cpp
	approot/resources.xml
	approot/wt_config.xml
)
target_link_libraries(${PROJECT_NAME}.wt Wt::DboSqlite3 Wt::HTTP Wt::Wt)

if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ALL
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
endif(DOXYGEN_FOUND)

# This is AGPL software: we distribute the source code
add_custom_target(src ALL COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.tar.gz" "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_dependencies(src ${PROJECT_NAME}.wt)

configure_file(${PROJECT_NAME}.wt.service.in ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.wt.service)
configure_file(wthttpd.in ${PROJECT_BINARY_DIR}/wthttpd)

install(TARGETS ${PROJECT_NAME}.wt DESTINATION ${CMAKE_INSTALL_PREFIX}/usr/bin)
install(DIRECTORY approot DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/${PROJECT_NAME})
install(DIRECTORY docroot DESTINATION ${CMAKE_INSTALL_PREFIX}/var/www/${PROJECT_NAME})

