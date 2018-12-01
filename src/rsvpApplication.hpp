/*!
	\mainpage ido: an RSVP web application for weddings
	\author Raf Pauwels
	\date 2017
	\copyright 
	\parblock
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, see <https://www.gnu.org/licenses/>.
	\endparblock
	
	\section Shortcomings
	
	This was developed in a short time so I took several shortcuts. 
	For example, table indication for guests is a string straight from the
	database, so these are not translated (our website was only in Dutch so
	I didn't bother). All other strings are externalized and thus translatable.
*/

#ifndef RSVPAPPLICATION_HPP_
#define RSVPAPPLICATION_HPP_

#include "party.hpp"

#include <Wt/WApplication.h>
#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTextArea.h>
#include <Wt/WTemplate.h>
#include <Wt/Dbo/Dbo.h>

#include <string>
#include <vector>
#include <utility>

//! Main application that takes a UUID and builds an interface.
class RsvpApplication : public Wt::WApplication
{
public:
	//! Sets up the database mapping and builds the interface.
	RsvpApplication(const Wt::WEnvironment& env);

private:
	Wt::Dbo::Session session_;
	Wt::Dbo::ptr<Party> party_;
	std::vector<Wt::WComboBox*> diets_;
	Wt::WContainerWidget *songContainer_;
	std::vector< std::pair<Wt::WLineEdit*, Wt::WLineEdit*> > songs_;
	Wt::WTextArea *remarks_;
	Wt::WTemplate *rsvp_;
	Wt::WPushButton *submit_;

	void submit();
	void songChanged();
	void addSong(const std::string& artist = "", const std::string& title = "");
	void setStatus();
};

#endif // RSVPAPPLICATION_HPP_
