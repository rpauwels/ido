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

#ifndef SONG_HPP_
#define SONG_HPP_

#include <string>

#include <Wt/Dbo/Dbo.h>

class Party;

//! A single song suggestion by a Party. They need not be unique.
class Song
{
public:
	//! Artist of the song
	std::string artist;
	
	//! Song title
	std::string title;
	
	//! Order of the song that was entered. Not of particular importance.
	int order;
	
	//! Party that entered the song
	Wt::Dbo::ptr<Party> party;

	Song() {};

	//! Only for Wt::Dbo. Describes persisted fields and relations.
	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, artist, "artist");
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::field(a, order, "order");
		Wt::Dbo::belongsTo(a, party, "party");
	}
};
#endif // SONG_HPP_
