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

#include "calendarResource.hpp"
#include "event.hpp"

#include <Wt/WDateTime.h>
#include <Wt/WResource.h>
#include <Wt/WString.h>
#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>

#include <iostream>

std::ostream& operator<<(std::ostream &stream, const Event &event) {
	Wt::WString dateTime = Wt::WDateTime::currentDateTime().toString("yyyyMMddTHHmmss");
	stream << "BEGIN:VEVENT"
		<< "\r\nUID:" << dateTime << "-" << event.summary << "@" << Wt::WString::tr("domain")
		<< "\r\nSUMMARY:" << event.summary
		<< "\r\nDTSTART:" << event.start.toString("yyyyMMddTHHmmss") << "Z"
		<< "\r\nDTEND:" << event.end.toString("yyyyMMddTHHmmss") << "Z"
		<< "\r\nDTSTAMP:" << dateTime << "Z";
	if (!event.location.empty())
		stream << "\r\nLOCATION:" << event.location;
	if (event.lat != 0.0 || event.lon != 0.0)
		stream << "\r\nGEO:" << event.lat << ";" << event.lon;
	stream << "\r\nEND:VEVENT";
	return stream;
}

CalendarResource::CalendarResource() 
	: Wt::WResource() {
	suggestFileName(Wt::WString::tr("icsFilename"));
}
	
CalendarResource::~CalendarResource() {
	beingDeleted();
}

void CalendarResource::addEvent(const Event &event) {
	events_.push_back(event);
}
	
void CalendarResource::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) {
	response.setMimeType("text/calendar");
	response.out() << "BEGIN:VCALENDAR" 
		<< "\r\nVERSION:2.0"
		<< "\r\nPRODID:https://" << Wt::WString::tr("domain") << "/";
	for (auto event: events_)
		response.out() << "\r\n" << event;
	response.out() << "\r\nEND:VCALENDAR\r" << std::endl;
}
