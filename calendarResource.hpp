#ifndef CALENDARRESOURCE_HPP_
#define CALENDARRESOURCE_HPP_

#include "party.hpp"

#include <Wt/WObject.h>
using Wt::WObject;

#include <Wt/WResource.h>
using Wt::WResource;

#include <Wt/Http/Request.h>
using Wt::Http::Request;

#include <Wt/Http/Response.h>
using Wt::Http::Response;

class CalendarResource : public WResource {
public:
	CalendarResource(InviteLevel level);
	~CalendarResource();
	void handleRequest(const Request& request, Response& response);
	
private:
	InviteLevel level_;
};

#endif // CALENDARRESOURCE_HPP_
