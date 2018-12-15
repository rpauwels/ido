project(ido)
cmake_minimum_required(VERSION 3.8)

find_package(Wt NO_DEFAULT_PATH REQUIRED)
find_package(Doxygen)
set(CMAKE_CXX_STANDARD 14)
set(SRC
	${PROJECT_SOURCE_DIR}/src/rsvpApplication.cpp
	${PROJECT_SOURCE_DIR}/src/adminApplication.cpp
	${PROJECT_SOURCE_DIR}/src/calendarResource.cpp
	${PROJECT_SOURCE_DIR}/src/event.cpp
)
set(ETC
	${PROJECT_SOURCE_DIR}/etc/resources.xml
	${PROJECT_SOURCE_DIR}/etc/resources_nl.xml
	${PROJECT_SOURCE_DIR}/etc/wt_config.xml
)
set(ARCHIVE ${PROJECT_NAME}-src.tar.xz)
set(SHARE
	share/style.css
	share/robbe.jpg
	share/robbe-blur.jpg
	share/animate.min.css
	share/economica-v5-latin-regular.woff2
	share/economica-v5-latin-regular.woff
	share/lato-v14-latin-regular.woff2
	share/lato-v14-latin-regular.woff
)
set(DOCROOT_FILES /style.css,/robbe.jpg,/robbe-blur.jpg,/animate.min.css,/economica-v5-latin-regular.woff2,/economica-v5-latin-regular.woff,/lato-v14-latin-regular.woff2,/lato-v14-latin-regular.woff)
set(SHARE_INSTALL ${SHARE} ${ARCHIVE})
#list(TRANSFORM SHARE_INSTALL PREPEND /)
#list(JOIN SHARE "," DOCROOT_FILES)
add_executable(${PROJECT_NAME}.wt ${SRC} ${ETC_SRC})
target_link_libraries(${PROJECT_NAME}.wt Wt::DboSqlite3 Wt::HTTP Wt::Wt)

if(DOXYGEN_FOUND)
	configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ALL
		${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	)
	install(DIRECTORY ${PROJECT_BINARY_DIR}/doc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME})
endif(DOXYGEN_FOUND)
add_custom_target(archive ALL ${CMAKE_COMMAND} -E tar cfJ ${PROJECT_BINARY_DIR}/${ARCHIVE} ${SRC} ${ETC})

configure_file(${PROJECT_NAME}.wt.service.in ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.wt.service)
configure_file(wthttpd.in ${PROJECT_BINARY_DIR}/wthttpd)

install(TARGETS ${PROJECT_NAME}.wt DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${ETC} DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/${PROJECT_NAME})
install(FILES ${PROJECT_BINARY_DIR}/wthttpd DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/wt)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.wt.service DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/systemd/system COMPONENT service)
install(FILES 
	${SHARE}
	${PROJECT_BINARY_DIR}/${ARCHIVE}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
