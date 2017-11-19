#include "calendarResource.hpp"
#include "party.hpp"

#include <Wt/WObject.h>
using Wt::WObject;

#include <Wt/WResource.h>
using Wt::WResource;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/Http/Request.h>
using Wt::Http::Request;

#include <Wt/Http/Response.h>
using Wt::Http::Response;

using std::endl;

CalendarResource::CalendarResource(InviteLevel level) 
	: WResource(), level_(level) {
	suggestFileName(WString::tr("icsFilename"));
}
	
CalendarResource::~CalendarResource() {
	beingDeleted();
}
	
void CalendarResource::handleRequest(const Request& request, Response& response) {
	response.setMimeType("text/calendar");
	response.out() << "BEGIN:VCALENDAR\n" 
		<< "VERSION:2.0\n"
		<< "PRODID:https://raf-en-vero.pauwels-cordier.be/\n"
		<< "METHOD:PUBLISH\n"
		<< WString::tr("ical.ceremony");
	if (level_ == InviteLevel::Full) {
		response.out() << WString::tr("ical.lunch") << "\n" 
			<< WString::tr("ical.photo");
	}
	response.out() << "\nEND:VCALENDAR" << endl;
}
