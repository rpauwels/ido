#include "event.hpp"

#include <Wt/WTemplate.h>
using Wt::WTemplate;

#include <string>
using std::to_string;

Event::Event() {}

void Event::fill(WTemplate &t) const {
	t.bindString("header", header);
	t.bindString("location", location);
	t.bindString("lat", to_string(lat));
	t.bindString("lon", to_string(lon));
	t.bindString("header", header);
}
