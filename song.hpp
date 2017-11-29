#ifndef SONG_HPP_
#define SONG_HPP_

//#include "party.hpp"

#include <string>
using std::string;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::ptr;
using Wt::Dbo::field;
using Wt::Dbo::belongsTo;

class Party;
class Song
{
public:
	string artist;
	string title;
	int order;
	ptr<Party> party;

	Song() {};

	template<class Action>
	void persist(Action& a)
	{
		field(a, artist, "artist");
		field(a, title, "title");
		field(a, order, "order");
		belongsTo(a, party, "party");
	}
};
#endif // SONG_HPP_
