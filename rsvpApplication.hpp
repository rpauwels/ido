#ifndef RSVPAPPLICATION_HPP_
#define RSVPAPPLICATION_HPP_

#include "party.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WContainerWidget.h>
using Wt::WContainerWidget;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WPushButton.h>
using Wt::WPushButton;

#include <Wt/WText.h>
using Wt::WText;

#include <Wt/WTextArea.h>
using Wt::WTextArea;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::Session;
using Wt::Dbo::ptr;

#include <vector>
using std::vector;

#include <utility>
using std::pair;

#include <string>
using std::string;

class RsvpApplication : public WApplication
{
public:
	RsvpApplication(const WEnvironment& env);

private:
	Session session_;
	ptr<Party> party_;
	vector<WComboBox*> diets_;
	WContainerWidget *songContainer_;
	vector< pair<WLineEdit*, WLineEdit*> > songs_;
	WTextArea *remarks_;
	WTemplate *rsvp_;
	WPushButton *submit_;

	void submit();
	void songChanged();
	void addSong(const string& artist = "", const string& title = "");
	void setStatus();
};

#endif // RSVPAPPLICATION_HPP_
