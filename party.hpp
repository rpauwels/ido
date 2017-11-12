#ifndef PARTY_HPP_
#define PARTY_HPP_

#include <string>
using std::string;

#include <Wt/WDate.h>
using Wt::WDate;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::ptr;
using Wt::Dbo::collection;
using Wt::Dbo::field;
using Wt::Dbo::hasMany;
using Wt::Dbo::ManyToOne;

#include <Wt/Dbo/WtSqlTraits.h>

enum class InviteLevel {
	Dessert = 0,
	Meal = 1,
	Full = 2
};

class Guest;
class Party
{
public:
	string uuid;
	string email;
	InviteLevel inviteLevel;
	WDate opened;
	string remarks;
	WDate confirmed;
	collection< ptr<Guest> > guests;

	Party();

	template<class Action>
	void persist(Action& a)
	{
		field(a, uuid, "uuid");
		field(a, email, "email");
		field(a, inviteLevel, "inviteLevel");
		field(a, opened, "opened");
		field(a, remarks, "remarks");
		field(a, confirmed, "confirmed");
		hasMany(a, guests, ManyToOne, "party");
	}
};
#endif // PARTY_HPP_
