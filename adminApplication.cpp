#include "adminApplication.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WPushButton.h>
using Wt::WPushButton;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/WText.h>
using Wt::WText;

#include <Wt/Mail/Message.h>
using Wt::Mail::Message;
using Wt::Mail::RecipientType;

#include <Wt/Mail/Mailbox.h>
using Wt::Mail::Mailbox;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::Transaction;
using Wt::Dbo::collection;
using Wt::Dbo::ptr;

#include <Wt/Dbo/backend/Sqlite3.h>
using Wt::Dbo::backend::Sqlite3;

using std::make_unique;
using std::unique_ptr;
using std::string;

AdminApplication::AdminApplication(const WEnvironment& env)
  : WApplication(env) {
	messageResourceBundle().use("resources");
	unique_ptr<Sqlite3> sqlite3(new Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(move(sqlite3));
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");
	
	pass_ = root()->addWidget(make_unique<WLineEdit>());
	pass_->setInline(false);
	pass_->setPlaceholderText(WString::tr("passwordPlaceholder"));
	create_ = root()->addWidget(make_unique<WPushButton>(WString::tr("create")));
	create_->setInline(false);
	create_->clicked().connect(this, &AdminApplication::create);
	invite_ = root()->addWidget(make_unique<WPushButton>(WString::tr("invite")));
	invite_->setInline(false);
	invite_->clicked().connect(this, &AdminApplication::invite);
}

void AdminApplication::create() {
	if (pass_->text() != WString::tr("password")) {
		status("Wrong password");
		return;
	}
	session_.createTables();
	log("info") << "Database created.";
}

void AdminApplication::invite() {
	if (pass_->text() != WString::tr("password")) {
		status("Wrong password");
		return;
	}
	Transaction transaction(session_);
	collection< ptr<Party> > parties = session_.find<Party>();
	for (const ptr<Party> &party: parties) {
		ptr<Guest> guest = party->guests.front();
		Message message;
		message.setFrom(Mailbox(WString::tr("fromAddress").toUTF8(), WString::tr("fromName")));
		message.addRecipient(RecipientType::To, Mailbox(party->email, guest->firstName + " " + guest->lastName));
		message.setSubject(WString::tr("invitation.subject"));
		message.setBody(WString::tr("invitation.body").arg(party->name));
		message.addHtmlBody(WString::tr("invitation.html").arg(party->name));
		client_.send(message);
		party.modify()->invited = WDate::currentDate();
		log("info") << "Sent invitation to " << party->email;
	}
	log("info") << "All invitations were sent.";
}

void AdminApplication::status(const string& status) {
	log("info") << status;
	root()->addWidget(make_unique<WText>(status))->setInline(false);
}
