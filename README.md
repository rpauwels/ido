# ido Wedding RSVP

ido is a simple web application that we created for managing our wedding RSVPs. It was quickly hacked together in a limited time, but can still serve as a Wt4 example application, since there are not that many to be found in the wild.

ido ships with the Wt HTTP daemon, meaning it runs as a standalone web server. Of course you can use a reverse proxy before it.

## Features

- Sending of tailored invitations to parties
- Indication of diet
- Song suggestions
- Dynamic iCalendar generation
- Sending of reminders with table numbers
- Responsive design!!1
- Animation!!11

## Build and Install

### Requirements

- Wt and Wt::Dbo 4.0.1 development packages
- CMake 3.6.3

### Instructions

1. In the project directory, create and enter a build directory: `mkdir build && cd build`
2. Locate the folder containing `wt-config.cmake` (and other CMake Wt files). You can usually find them in `/usr/lib/cmake/wt`.
3. Configure using CMake and use the previous value for Wt_DIR: `cmake -DWt_DIR=/usr/lib/cmake/wt ..`
4. Make the project: `make -j5`
5. Install the project: `make install`

## Run

### Requirements

- Wt and Wt::Dbo 4.0.1
- Sqlite3
- systemd (service)

### Instructions

Running the web server manually in the foreground: `ido.wt --docroot /var/www/ido --http-listen 0.0.0.0:8080`

Enabling the web server daemon: `sudo systemctl enable ido.wt`

Starting the web server daemon: `sudo systemctl start ido.wt`

## License
