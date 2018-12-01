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
	configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ALL
		${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	)
endif(DOXYGEN_FOUND)

# This is AGPL software: we distribute the source code
add_custom_target(src ALL COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-src.tar.gz" "${PROJECT_SOURCE_DIR}/{CMakeLists.txt,COPYING,Doxyfile.in,README.md,approot,docroot,src}")

add_dependencies(src ${PROJECT_NAME}.wt)

configure_file(${PROJECT_NAME}.wt.service.in ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.wt.service)
configure_file(wthttpd.in ${PROJECT_BINARY_DIR}/wthttpd)

install(TARGETS ${PROJECT_NAME}.wt DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY approot DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/${PROJECT_NAME})
install(FILES ${PROJECT_BINARY_DIR}/wthttpd DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/wt)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.wt.service DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/systemd/system COMPONENT service)
install(FILES 
	docroot/style.css
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}-src.tar.gz 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
install(DIRECTORY 
	docroot/animate.css 
	docroot/fonts 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
install(DIRECTORY ${PROJECT_BINARY_DIR}/doc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME})
