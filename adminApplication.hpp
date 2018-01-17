#ifndef ADMINAPPLICATION_HPP_
#define ADMINAPPLICATION_HPP_

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WPushButton.h>
using Wt::WPushButton;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::Session;

using std::string;

class AdminApplication : public WApplication {
public:
	AdminApplication(const WEnvironment& env);

private:
	Session session_;
	WPushButton *create_;
	WComboBox *inviteLevel_;
	WPushButton *invite_;
	void create();
	void invite();
	void status(const string& status);
	void sendUpdates();
};

#endif // ADMINAPPLICATION_HPP_
