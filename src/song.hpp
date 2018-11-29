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
