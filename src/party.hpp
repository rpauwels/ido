#ifndef PARTY_HPP_
#define PARTY_HPP_

#include "event.hpp"

#include <Wt/WDateTime.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include <string>

//! Whether a party is invited for only dessert, the full meal or all events.
enum class InviteLevel {
	Dessert = 0,
	Meal = 1,
	Full = 2
};

class Guest;
class Song;

//! Aggregation of guests, having an invite level
/*!
 * A Party is an aggregation of guests, because usually guests want to respond
 * as a family instead of individually. Parties aggregate the e-mail 
 * addresses of their members, but have the same UUID. The list of events is 
 * actually filled by the InviteLevel, so this information is not normalized.
 */
class Party
{
public:
	std::string uuid;
	std::string name;
	InviteLevel inviteLevel;
	Wt::WDateTime invited;
	Wt::WDateTime opened;
	std::string remarks;
	Wt::WDateTime confirmed;
	Wt::Dbo::collection< Wt::Dbo::ptr<Guest> > guests;
	Wt::Dbo::collection< Wt::Dbo::ptr<Event> > events;
	Wt::Dbo::collection< Wt::Dbo::ptr<Song> > songs;
	
	Party() {};

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, uuid, "uuid");
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::field(a, inviteLevel, "inviteLevel");
		Wt::Dbo::field(a, invited, "invited");
		Wt::Dbo::field(a, opened, "opened");
		Wt::Dbo::field(a, remarks, "remarks");
		Wt::Dbo::field(a, confirmed, "confirmed");
		Wt::Dbo::hasMany(a, guests, Wt::Dbo::ManyToOne, "party");
		Wt::Dbo::hasMany(a, events, Wt::Dbo::ManyToMany, "event_party");
		Wt::Dbo::hasMany(a, songs, Wt::Dbo::ManyToOne, "party");
	}
};
#endif // PARTY_HPP_
