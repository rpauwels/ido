#ifndef CALENDARRESOURCE_HPP_
#define CALENDARRESOURCE_HPP_

#include "event.hpp"

#include <Wt/WDateTime.h>
using Wt::WDateTime;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/WResource.h>
using Wt::WResource;

#include <Wt/Http/Request.h>
using Wt::Http::Request;

#include <Wt/Http/Response.h>
using Wt::Http::Response;

#include <vector>
using std::vector;

class CalendarResource : public WResource {
public:
	CalendarResource();
	~CalendarResource();
	void handleRequest(const Request& request, Response& response);
	void addEvent(const Event &event);
	
private:
	vector<Event> events_;
};

#endif // CALENDARRESOURCE_HPP_
