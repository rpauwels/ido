/*
	ido: an RSVP web application for weddings
	Copyright (C) 2017  Raf Pauwels

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, see <https://www.gnu.org/licenses/>.
*/

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
