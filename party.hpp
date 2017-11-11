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

class Guest;
class Party
{
public:
	string uuid;
	string email;
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
		field(a, opened, "opened");
		field(a, remarks, "remarks");
		field(a, confirmed, "confirmed");
		hasMany(a, guests, ManyToOne, "party");
	}
};
#endif // PARTY_HPP_
