#ifndef PARTY_HPP_
#define PARTY_HPP_

#include "event.hpp"
#include "song.hpp"

#include <string>
using std::string;

#include <Wt/WDateTime.h>
using Wt::WDateTime;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::ptr;
using Wt::Dbo::collection;
using Wt::Dbo::field;
using Wt::Dbo::hasMany;
using Wt::Dbo::ManyToOne;
using Wt::Dbo::ManyToMany;

#include <Wt/Dbo/WtSqlTraits.h>

enum class InviteLevel {
	Dessert = 0,
	Meal = 1,
	Full = 2
};

class Guest;
class Song;
class Party
{
public:
	string uuid;
	string name;
	string email;
	InviteLevel inviteLevel;
	WDateTime invited;
	WDateTime opened;
	string remarks;
	WDateTime confirmed;
	collection< ptr<Guest> > guests;
	collection< ptr<Event> > events;
	collection< ptr<Song> > songs;
	
	Party() {};

	template<class Action>
	void persist(Action& a)
	{
		field(a, uuid, "uuid");
		field(a, name, "name");
		field(a, email, "email");
		field(a, inviteLevel, "inviteLevel");
		field(a, invited, "invited");
		field(a, opened, "opened");
		field(a, remarks, "remarks");
		field(a, confirmed, "confirmed");
		hasMany(a, guests, ManyToOne, "party");
		hasMany(a, events, ManyToMany, "event_party");
		hasMany(a, songs, ManyToOne, "party");
	}
};
#endif // PARTY_HPP_
