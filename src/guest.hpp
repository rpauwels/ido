#ifndef GUEST_HPP_
#define GUEST_HPP_

#include <string>
using std::string;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::ptr;
using Wt::Dbo::field;
using Wt::Dbo::belongsTo;

//! Diet of the guest, or None if (s)he cannot make it
enum class Diet {
	None = 0,
	Carnivore = 1,
	Herbivore = 2,
	Pollotarian = 3
};

class Party;

//! One member of a party
/*! 
 * A party member that has a name and e-mail address, party-specific order,
 * a diet (including whether (s)he can make it) and a place (table).
 * E-mail and place are only for internal organisation and are aggregated
 * at the party level.
 */
class Guest
{
public:
	string firstName;
	string lastName;
	string email;
	int order;
	Diet diet;
	ptr<Party> party;
	string place;

	Guest() {};

	template<class Action>
	void persist(Action& a)
	{
		field(a, firstName, "first_name");
		field(a, lastName, "last_name");
		field(a, email, "email");
		field(a, order, "order");
		field(a, diet, "diet");
		belongsTo(a, party, "party");
		field(a, place, "place");
	}
};
#endif // GUEST_HPP_
