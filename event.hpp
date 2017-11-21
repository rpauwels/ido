#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <string>
using std::string;

#include <Wt/WDateTime.h>
using Wt::WDateTime;

#include <Wt/WTemplate.h>
using Wt::WTemplate;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::ptr;
using Wt::Dbo::collection;
using Wt::Dbo::field;
using Wt::Dbo::hasMany;
using Wt::Dbo::ManyToMany;

#include <Wt/Dbo/WtSqlTraits.h>

class Party;
class Event
{
public:
	string header;
	string summary;
	string location;
	float lat;
	float lon;
	WDateTime start;
	WDateTime end;
	collection< ptr<Party> > parties;

	Event();
	void fill(WTemplate &t) const;

	template<class Action>
	void persist(Action& a)
	{
		field(a, header, "header");
		field(a, summary, "summary");
		field(a, location, "location");
		field(a, lat, "lat");
		field(a, lon, "lon");
		field(a, start, "start");
		field(a, end, "end");
		hasMany(a, parties, ManyToMany, "event_party");
	}
};
#endif // EVENT_HPP_
