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
