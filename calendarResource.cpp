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
		<< "\nUID:" << dateTime << "-" << event.summary << "@" << WString::tr("domain")
		<< "\nSUMMARY:" << event.summary
		<< "\nDTSTART:" << event.start.toString("yyyyMMddTHHmmss")
		<< "\nDTEND:" << event.end.toString("yyyyMMddTHHmmss")
		<< "\nDTSTAMP:" << dateTime;
	if (!event.location.empty())
		stream << "\nLOCATION:" << event.location
			<< "\nGEO:" << event.lat << "," << event.lon;
	stream << "\nEND:VEVENT";
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
		<< "\nVERSION:2.0"
		<< "\nPRODID:https://" << WString::tr("domain") << "/"
		<< "\nMETHOD:PUBLISH";
	for (const Event &event: events_)
		response.out() << "\n" << event;
	response.out() << "\nEND:VCALENDAR" << endl;
}
