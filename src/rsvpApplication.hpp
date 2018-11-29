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
