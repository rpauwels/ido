#ifndef RSVPAPPLICATION_HPP_
#define RSVPAPPLICATION_HPP_

#include "party.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WPushButton.h>
using Wt::WPushButton;

#include <Wt/WText.h>
using Wt::WText;

#include <Wt/Mail/Client.h>
using Wt::Mail::Client;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::Session;
using Wt::Dbo::ptr;

#include <vector>
using std::vector;

#include <utility>
using std::pair;

class RsvpApplication : public WApplication
{
public:
	RsvpApplication(const WEnvironment& env);

private:
	Session session_;
	Client client_;
	ptr<Party> party_;
	vector<WComboBox*> diets_;
	vector<pair<WLineEdit*, WLineEdit*>> songs_;
	WLineEdit *remarks_;
	WText *status_;
	WPushButton *submit_;

	void submit();
	void songChanged();
};

#endif // RSVPAPPLICATION_HPP_
