/*
	ido: an RSVP web application for weddings
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
*/

#ifndef CALENDARRESOURCE_HPP_
#define CALENDARRESOURCE_HPP_

#include "event.hpp"

#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/WResource.h>

#include <vector>

//! iCalendar generator based on the visible events
class CalendarResource : public Wt::WResource {
public:
	CalendarResource();
	~CalendarResource();
	
	//! Generate a new iCalendar file upon download request
	void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
	
	//! Add a new event to the list of generated events
	void addEvent(const Event &event);
private:
	std::vector<Event> events_;
};

#endif // CALENDARRESOURCE_HPP_
