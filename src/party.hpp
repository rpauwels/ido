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
	//! Generated UUID used as parameter as main identification.
	std::string uuid;
	
	//! Used as greeting in e-mails.
	std::string name;
	
	//! At what level a party is invited.
	InviteLevel inviteLevel;
	
	//! Date and time of invitation by e-mail.
	Wt::WDateTime invited;
	
	//! Date and time that the UUID was last used to open the website.
	Wt::WDateTime opened;
	
	//! Remarks that the party can enter when sending the form.
	std::string remarks;
	
	//! Date and time that the guest confirmed.
	Wt::WDateTime confirmed;
	
	//! Party members.
	Wt::Dbo::collection< Wt::Dbo::ptr<Guest> > guests;
	
	//! Events to which the party is invited.
	Wt::Dbo::collection< Wt::Dbo::ptr<Event> > events;
	
	//! Song suggestions that the party sent in.
	Wt::Dbo::collection< Wt::Dbo::ptr<Song> > songs;
	
	Party() {};

	//! Only for Wt::Dbo. Describes persisted fields and relations.
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
