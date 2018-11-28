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
	std::string header;
	std::string summary;
	std::string location;
	float lat;
	float lon;
	Wt::WDateTime start;
	Wt::WDateTime end;
	Wt::Dbo::collection< Wt::Dbo::ptr<Party> > parties;

	Event();
	
	//! Fill a WTemplate based on the event information
	void fill(Wt::WTemplate &t) const;

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
