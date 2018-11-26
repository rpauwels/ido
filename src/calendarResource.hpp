#ifndef CALENDARRESOURCE_HPP_
#define CALENDARRESOURCE_HPP_

#include "event.hpp"

#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/WResource.h>

#include <vector>

class CalendarResource : public Wt::WResource {
public:
	CalendarResource();
	~CalendarResource();
	void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
	void addEvent(const Event &event);
	
private:
	std::vector<Event> events_;
};

#endif // CALENDARRESOURCE_HPP_
