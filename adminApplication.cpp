#include "adminApplication.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WGroupBox.h>
using Wt::WGroupBox;

#include <Wt/WLogger.h>
using Wt::log;

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
	
	create_ = root()->addWidget(make_unique<WPushButton>(WString::tr("create")));
	create_->setInline(false);
	create_->clicked().connect(this, &AdminApplication::create);
	auto inviteGroup = root()->addWidget(make_unique<WGroupBox>());
	inviteLevel_ = inviteGroup->addWidget(make_unique<WComboBox>());
	inviteLevel_->addItem(WString::tr("dessert"));
	inviteLevel_->addItem(WString::tr("meal"));
	inviteLevel_->addItem(WString::tr("full"));
	invite_ = inviteGroup->addWidget(make_unique<WPushButton>(WString::tr("invite")));
	invite_->setInline(false);
	invite_->clicked().connect(this, &AdminApplication::invite);
}

void AdminApplication::create() {
	session_.createTables();
	log("info") << "Database created.";
}

void AdminApplication::invite() {
	Transaction transaction(session_);
	collection< ptr<Party> > parties = session_.find<Party>().where("inviteLevel = ?").bind(inviteLevel_->currentIndex());
	for (const ptr<Party> &party: parties) {
		ptr<Guest> guest = party->guests.front();
		Message message;
		message.setFrom(Mailbox(WString::tr("fromAddress").toUTF8(), WString::tr("fromName")));
		message.addRecipient(RecipientType::To, Mailbox(party->email, guest->firstName + " " + guest->lastName));
		message.setSubject(WString::tr("invitation.subject"));
		message.setBody(WString::tr("invitation.body").arg(party->name));
		message.addHtmlBody(WString::tr("invitation.html").arg(party->name));
		client_.connect();
		client_.send(message);
		party.modify()->invited = WDate::currentDate();
		log("info") << "Sent invitation to " << party->email;
	}
	log("info") << "All invitations were sent.";
}

