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

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <Wt/WDateTime.h>
#include <Wt/WTemplate.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include <string>

class Party;

//! A single event, with a place and start and end time.
class Event
{
public:
	//! Short name of the event, also used for the image name
	std::string header;
	
	//! Single-word description
	std::string summary;
	
	//! Full address
	std::string location;
	
	//! Location latitude
	float lat;
	
	//! Location longitude
	float lon;
	
	//! Start time
	Wt::WDateTime start;
	
	//! End time (only visible in iCalendar format)
	Wt::WDateTime end;
	
	//! Invited parties to this event
	Wt::Dbo::collection< Wt::Dbo::ptr<Party> > parties;

	Event();
	
	//! Fill a WTemplate based on the event information
	void fill(Wt::WTemplate &t) const;

	//! Only for Wt::Dbo. Describes persisted fields and relations.
	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, header, "header");
		Wt::Dbo::field(a, summary, "summary");
		Wt::Dbo::field(a, location, "location");
		Wt::Dbo::field(a, lat, "lat");
		Wt::Dbo::field(a, lon, "lon");
		Wt::Dbo::field(a, start, "start");
		Wt::Dbo::field(a, end, "end");
		Wt::Dbo::hasMany(a, parties, Wt::Dbo::ManyToMany, "event_party");
	}
};
#endif // EVENT_HPP_
