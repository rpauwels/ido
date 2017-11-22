#include "event.hpp"

#include <boost/algorithm/string.hpp>
using boost::replace_all;

#include <Wt/WTemplate.h>
using Wt::WTemplate;

#include <string>
using std::string;
using std::to_string;

Event::Event() {}

void Event::fill(WTemplate &t) const {
	t.bindString("header", header);
	string location_br = location;
	replace_all(location_br, ", ", "<br/>");
	t.bindString("location", location_br);
	t.bindString("lat", to_string(lat));
	t.bindString("lon", to_string(lon));
	t.bindString("start", start.toString("HH:mm"));
}
