# ido Wedding RSVP Application #

ido is a simple web application for managing wedding RSVPs. It was quickly hacked together in a limited time, but can still serve as a Wt4 example application, since there are not too many to be found in the wild.

ido ships with the Wt HTTP daemon, meaning it runs as a standalone web server. Of course you can use a reverse proxy before it.

## Features ##

- Sending of general invitations to parties with individualized links
- Indication of diet (depending on invitation)
- Song suggestions
- Dynamic iCalendar generation
- Sending of reminders with table numbers
- Responsive design and animation (OMG!)

## Build and Install ##

You will need:

- Wt and Wt::Dbo 4.0.1 development packages
- CMake 3.6.3 or higher (probably works with far older versions if you change CMAKE\_MIN\_RELEASE)
- Doxygen (optional)

### Instructions ###

1. In the project directory, create and enter a build directory: `mkdir build && cd build`
2. Locate the folder containing `wt-config.cmake` (and other CMake Wt files). You can usually find them in `/usr/lib/cmake/wt`.
3. Configure using CMake and use the previous value for `Wt_DIR`. Optionally, use `CMAKE_INSTALL_PREFIX` if you want to install somewhere else than the default `/usr/local`: `cmake -DWt_DIR=/usr/lib/cmake/wt -DCMAKE_INSTALL_PREFIX=/ ..`
4. Make the project: `make -j5`
5. Install the project: `make install`

## Running ##

### Requirements ###

- Wt and Wt::Dbo 4.0.1
- Sqlite3
- systemd (service)

### Getting Started ###

The project ships with a `${CMAKE_INSTALL_PREFIX}/etc/wt/wthttpd` file that contains sensible default startup parameters. If this is your only Wt application, you can copy it into `/etc/wt/wthttpd` and simply run `ido.wt`. Otherwise, use these values as the startup parameters to run the web server manually in the foreground:

	ido.wt --docroot /usr/local/share/ido --approot /usr/local/etc/ido --http-listen 0.0.0.0:8080

### Application Paths ###

In a default Wt installation, the Wt resources dir is installed to `${CMAKE_INSTALL_PREFIX}/share/Wt/resources`. These files are served from `/resources/`. Ido builds its styling upon the default style and Bootstrap theme. It can be configured with `--resources-dir`.

Additionally, the webserver serves files from the docroot. These are installed to `${CMAKE_INSTALL_PREFIX}/share/ido`. They are application-specific files the user agent needs (CSS stylesheets, fonts, images). This can be configured with `--docroot`. In addition to the path, this parameter can also have a list of files to serve them into the internal path. This is the case by default.

The approot contains files that only the webserver needs. You can find them in `${CMAKE_INSTALL_PREFIX}/etc/ido`. This is mainly the application configuration and translation resource files. The folder can be read-only. You can configure it with `--approot`.

Finally, the working directory is used for read-write webserver resources. That is only the `ido.db` SQLite3-database, which is created (but not initialized) automatically if not present.

### Service ###

Enabling the web server daemon: `sudo systemctl enable ido.wt`

Starting the web server daemon: `sudo systemctl start ido.wt`

### Database Setup ###

The administration application is reachable through /admin. It has no authentication by itself so be careful not to expose this directly. In my setup, I depended on a reverse proxy for authentication. 

The administration interface allows to create the initial SQLite database file. Remember that it does not allow for data entry. You are expected to create events, parties and guests directly in the database using other tools. I used [DB Browser for SQLite](https://sqlitebrowser.org/) for this.

Providing a `uuid` query parameter is required. If it is absent when opening the root page, due to the early return only the title and image is shown. You can simply generate the necessary UUIDs for the party table in SQLite: `lower(hex(randomblob(16)))`

You will invite most parties to a limited number of different 'event sets'. They can be entered through the a party's inviteLevel. This field is only directly used as groups for e-mailing in the administrative application, and diet selection (so parties not invited to dinner get only present/absent choice). This way, for special cases you are still free to change the events to which they are invited. You need to only use inviteLevel, then you can link events with statements like this (to associate all parties with inviteLevel 1 or higher to event 3): 

		insert into event_party (event_id, party_id)
			select 3, id 
			from party 
			where inviteLevel >= 1;

## License ##

Copyright (C) 2017  Raf Pauwels

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, see <https://www.gnu.org/licenses/>.
