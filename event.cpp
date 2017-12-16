#include "event.hpp"

#include <boost/algorithm/string.hpp>
using boost::replace_all;

#include <Wt/WLocalDateTime.h>

#include <Wt/WTemplate.h>
using Wt::WTemplate;

#include <string>
using std::string;
using std::to_string;

Event::Event() {}

void Event::fill(WTemplate &t) const {
	t.bindString("header", header);
	t.bindString("summary", summary);
	t.bindString("start", start.toLocalTime().toString("HH:mm"));
	if (lat == 0.0 && lon == 0.0) {
		t.setCondition("if-no-geo", true);
	} else {
		t.setCondition("if-geo", true);
		t.bindString("lat", to_string(lat));
		t.bindString("lon", to_string(lon));
	}
	string location_br = location;
	replace_all(location_br, ", ", "<br/>");
	t.bindString("location", location_br);
}
