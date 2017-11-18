#include "calendarResource.hpp"

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
	response.out() << "BEGIN:VCALENDAR" << endl;
	response.out() << "VERSION:2.0" << endl;
	response.out() << "END:VCALENDAR" << endl;
}
