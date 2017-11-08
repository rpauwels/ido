#ifndef GUEST_HPP_
#define GUEST_HPP_

#include <string>
using std::string;

#include <Wt/WDate.h>
using Wt::WDate;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::ptr;
//using Wt::Dbo::collection;
using Wt::Dbo::field;
using Wt::Dbo::belongsTo;
//using Wt::Dbo::ManyToOne;

enum class Diet {
	None = 0,
	Carnivore = 1,
	Herbivore = 2
};

class Party;
class Guest
{
public:
	string firstName;
	string lastName;
	Diet diet;
	ptr<Party> party;

	Guest();

	template<class Action>
	void persist(Action& a)
	{
		field(a, firstName, "first_name");
		field(a, lastName, "last_name");
		field(a, diet, "diet");
		belongsTo(a, party, "party");
	}
};
#endif // GUEST_HPP_
