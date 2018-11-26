#ifndef ADMINAPPLICATION_HPP_
#define ADMINAPPLICATION_HPP_

#include <Wt/WApplication.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/Dbo/Dbo.h>

using std::string;

class AdminApplication : public Wt::WApplication {
public:
	AdminApplication(const Wt::WEnvironment& env);

private:
	Wt::Dbo::Session session_;
	Wt::WPushButton *create_;
	Wt::WComboBox *inviteLevel_;
	Wt::WPushButton *invite_;
	void create();
	void invite();
	void status(const std::string& status);
	void sendUpdates();
};

#endif // ADMINAPPLICATION_HPP_
