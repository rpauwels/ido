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

#ifndef GUEST_HPP_
#define GUEST_HPP_

#include <Wt/Dbo/Dbo.h>

#include <string>

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
	//! Given name, used in RSVP form
	std::string firstName;
	
	//! Last name, used in RSVP form
	std::string lastName;
	
	//! E-mail address. One e-mail is sent for the whole party, with all guest addresses.
	std::string email;
	
	//! Defines ordering in the party
	int order;
	
	//! Most important field for RSVP: whether (s)he can make it, and the diet.
	Diet diet;
	
	//! Associated party. A single guest is also associated with a party.
	Wt::Dbo::ptr<Party> party;
	
	//! Table name. Distinct values are aggregated at the party level.
	std::string place;

	Guest() {};

	//! Only for Wt::Dbo. Describes persisted fields and relations.
	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, firstName, "first_name");
		Wt::Dbo::field(a, lastName, "last_name");
		Wt::Dbo::field(a, email, "email");
		Wt::Dbo::field(a, order, "order");
		Wt::Dbo::field(a, diet, "diet");
		Wt::Dbo::belongsTo(a, party, "party");
		Wt::Dbo::field(a, place, "place");
	}
};
#endif // GUEST_HPP_
