#include "event.hpp"

#include <Wt/WLocalDateTime.h>
#include <Wt/WTemplate.h>

#include <boost/algorithm/string.hpp>
#include <string>

Event::Event() {}

void Event::fill(Wt::WTemplate &t) const {
	t.bindString("header", header);
	t.bindString("summary", summary);
	t.bindString("start", start.toLocalTime().toString("HH:mm"));
	if (lat == 0.0 && lon == 0.0) {
		t.setCondition("if-no-geo", true);
	} else {
		t.setCondition("if-geo", true);
		t.bindString("lat", std::to_string(lat));
		t.bindString("lon", std::to_string(lon));
	}
	std::string location_br = location;
	boost::replace_all(location_br, ", ", "<br/>");
	t.bindString("location", location_br);
}
