#ifndef ADMINAPPLICATION_HPP_
#define ADMINAPPLICATION_HPP_

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

using std::string;

class AdminApplication : public WApplication
{
public:
	AdminApplication(const WEnvironment& env);

private:
	Session session_;
	Client client_;
	WLineEdit *pass_;
	WPushButton *create_;
	WComboBox *inviteLevel_;
	WPushButton *invite_;
	void create();
	void invite();
	void status(const string& status);
};

#endif // RSVPAPPLICATION_HPP_
