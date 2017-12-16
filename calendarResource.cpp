#include "calendarResource.hpp"
#include "event.hpp"

#include <Wt/WObject.h>
using Wt::WObject;

#include <Wt/WResource.h>
using Wt::WResource;

#include <Wt/WDateTime.h>
using Wt::WDateTime;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/Http/Request.h>
using Wt::Http::Request;

#include <Wt/Http/Response.h>
using Wt::Http::Response;

#include <iostream>
using std::ostream;
using std::endl;

ostream& operator<<(ostream &stream, const Event &event) {
	WString dateTime = WDateTime::currentDateTime().toString("yyyyMMddTHHmmss");
	stream << "BEGIN:VEVENT"
		<< "\r\nUID:" << dateTime << "-" << event.summary << "@" << WString::tr("domain")
		<< "\r\nSUMMARY:" << event.summary
		<< "\r\nDTSTART:" << event.start.toString("yyyyMMddTHHmmss") << "Z"
		<< "\r\nDTEND:" << event.end.toString("yyyyMMddTHHmmss") << "Z"
		<< "\r\nDTSTAMP:" << dateTime << "Z";
	if (!event.location.empty())
		stream << "\r\nLOCATION:" << event.location;
	if (event.lat != 0.0 || event.lon != 0.0)
		stream << "\r\nGEO:" << event.lat << ";" << event.lon;
	stream << "\r\nEND:VEVENT";
}

CalendarResource::CalendarResource() 
	: WResource() {
	suggestFileName(WString::tr("icsFilename"));
}
	
CalendarResource::~CalendarResource() {
	beingDeleted();
}

void CalendarResource::addEvent(const Event &event) {
	events_.push_back(event);
}
	
void CalendarResource::handleRequest(const Request& request, Response& response) {
	response.setMimeType("text/calendar");
	response.out() << "BEGIN:VCALENDAR" 
		<< "\r\nVERSION:2.0"
		<< "\r\nPRODID:https://" << WString::tr("domain") << "/";
	for (const Event &event: events_)
		response.out() << "\r\n" << event;
	response.out() << "\r\nEND:VCALENDAR\r" << endl;
}
