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

#include <Wt/Mail/Client.h>
using Wt::Mail::Client;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::Session;
using Wt::Dbo::ptr;

class RsvpApplication : public WApplication
{
public:
	RsvpApplication(const WEnvironment& env, bool embedded);

private:
	Session session_;
	Client client_;
	ptr<Party> party_;
	WComboBox *diet_;
	WLineEdit *remarks_;
	WPushButton *submit_;
	void submit();
};

#endif // RSVPAPPLICATION_HPP_
