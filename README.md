# ido Wedding RSVP Application #

ido is a simple web application for managing wedding RSVPs. ido ships with the Wt HTTP daemon, meaning it runs as a standalone web server. Of course you can use it in combination with a reverse proxy.

## Features ##

- Sending of general invitations to parties with individualized links
- Optional indication of diet
- Song suggestions
- iCalendar file generation
- Sending of reminders with table numbers
- Responsive design and animation, ZOMG

## Build and Install ##

You will need:

- Wt and Wt::Dbo 4
- CMake 3.6.3 or higher
- Doxygen (optional)

### Instructions ###

1. In the project directory, create and enter a build directory: `mkdir build && cd build`
2. Locate the folder containing `wt-config.cmake` (and other CMake Wt files). You can usually find them in `/usr/lib/cmake/wt`.
3. Configure using CMake and use the previous value for `Wt_DIR`. Optionally, use `CMAKE_INSTALL_PREFIX` if you want to install somewhere else than the default `/usr/local`: `cmake -DWt_DIR=/usr/lib/cmake/wt -DCMAKE_INSTALL_PREFIX=/ ..`
4. Make the project: `make -j5`
5. Install the project (use `sudo` if necessary): `make install`

## Running ##

### Requirements ###

- Wt and Wt::Dbo 4
- Sqlite3
- systemd (service)

### Getting Started ###

In what follows, `${CMAKE_INSTALL_PREFIX}` refers to the value of the CMake parameter designating the installation directory (most likely `/usr/local` if you did not set it during build).

The project ships with a `${CMAKE_INSTALL_PREFIX}/etc/wt/wthttpd` file that contains sensible default startup parameters. If this is your only Wt application, you can copy it into `/etc/wt/wthttpd` and simply run `ido.wt`. Otherwise, use these values as the startup parameters to run the web server manually in the foreground:

		ido.wt --docroot ${CMAKE_INSTALL_PREFIX}/share/ido --approot ${CMAKE_INSTALL_PREFIX}/etc/ido --http-listen 0.0.0.0:8080

### Database Setup ###

Direct your browser to

		http://localhost:8080/admin

You have now opened the very minimal admin interface. It has no authentication by itself so be careful not to directly expose this path to the outside world. In my setup, I depended on a reverse proxy for authentication. 

The administration interface allows to create the initial SQLite database file. Note that it does not allow for data entry. You are expected to create events, parties and guests directly in the database. That does not mean that you need to write the entire thing using SQL, you can use tools like [DB Browser for SQLite](https://sqlitebrowser.org/).

An empty `ido.db` database file will have been created in your working directory. Click 'Create tables' in the admin interface to create the structure that the application expects. Now you need to fill the database file with events, parties and guests. See the documentation for detailed descriptions of these tables and their columns (reflected as classes and fields using Wt::Dbo). Examples for each:

		INSERT INTO party (version, uuid, name, inviteLevel, remarks)
		VALUES (0, lower(hex(randomblob(16))),'Incredibles',0,'');

		INSERT INTO guest (version, first_name, last_name, email, order, diet)
		VALUES (0, 'Mr.', 'Incredible', 'mr.incredible@example.org', 0)

Providing a `uuid` query parameter is required. It really should not have been called uuid, because it can be any simple unique identifier and I just used `lower(hex(randomblob(16)))`. If it is absent when opening the root page, page construction simply returns early and only the title and image is shown.

You will invite most parties to a limited number of different 'event sets'. They can be entered through the a party's inviteLevel. This field is only directly used as groups for e-mailing in the administrative application, and diet selection (so parties not invited to dinner get only a present/absent choice). This way, for special cases you can still change the events to which they are invited. You need to only use inviteLevel, then you can link events with statements like this (to associate all parties with inviteLevel 1 or higher to event with id 3): 

		insert into event_party (event_id, party_id)
			select 3, id 
			from party 
			where inviteLevel >= 1;

### Application Paths ###

In a default Wt installation, the Wt resources dir is installed to `${CMAKE_INSTALL_PREFIX}/share/Wt/resources`. These files are served from `/resources/`. Ido builds its styling upon the default style and Bootstrap theme. It can be configured with `--resources-dir`.

Additionally, the webserver serves files from the docroot. These are installed to `${CMAKE_INSTALL_PREFIX}/share/ido`. They are application-specific files the user agent needs (CSS stylesheets, fonts, images). This can be configured with `--docroot`. In addition to the path, this parameter can also have a list of files to serve them into the internal path. This is the case by default.

The approot contains files that only the webserver needs. You can find them in `${CMAKE_INSTALL_PREFIX}/etc/ido`. This is mainly the application configuration and translation resource files. The folder can be read-only. You can configure this using `--approot`.

Finally, the working directory is used for read-write webserver resources. That is only the `ido.db` SQLite3-database, which is created (but not initialized) automatically if not present.

### Service ###

Enabling the web server daemon: `sudo systemctl enable ido.wt`

Starting the web server daemon: `sudo systemctl start ido.wt`

