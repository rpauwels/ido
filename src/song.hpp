#ifndef SONG_HPP_
#define SONG_HPP_

#include <string>

#include <Wt/Dbo/Dbo.h>

class Party;
class Song
{
public:
	std::string artist;
	std::string title;
	int order;
	Wt::Dbo::ptr<Party> party;

	Song() {};

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
