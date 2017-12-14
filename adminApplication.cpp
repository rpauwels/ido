#include "adminApplication.hpp"
#include "guest.hpp"
#include "party.hpp"
#include "song.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WDateTime.h>
using Wt::WDateTime;

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

#include <Wt/Mail/Client.h>
using Wt::Mail::Client;

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

using std::unique_ptr;
using std::string;

AdminApplication::AdminApplication(const WEnvironment& env)
  : WApplication(env) {
	messageResourceBundle().use("resources");
	unique_ptr<Sqlite3> sqlite3(new Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(move(sqlite3));
	session_.mapClass<Event>("event");
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");
	session_.mapClass<Song>("song");
	
	create_ = root()->addNew<WPushButton>(WString::tr("create"));
	create_->setInline(false);
	create_->clicked().connect(this, &AdminApplication::create);
	auto inviteGroup = root()->addNew<WGroupBox>();
	inviteLevel_ = inviteGroup->addNew<WComboBox>();
	inviteLevel_->addItem(WString::tr("dessert"));
	inviteLevel_->addItem(WString::tr("meal"));
	inviteLevel_->addItem(WString::tr("full"));
	invite_ = inviteGroup->addNew<WPushButton>(WString::tr("invite"));
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
	Client client;
	if (!client.connect())
		log("error") << "Could not connect to SMTP server";
	for (const ptr<Party> &party: parties) {
		ptr<Guest> guest = party->guests.front();
		Message message;
		message.setFrom(Mailbox(WString::tr("fromAddress").toUTF8(), WString::tr("fromName")));
		for (const ptr<Guest> &guest: party->guests) {
			if (!guest->email.empty())
				message.addRecipient(RecipientType::To, Mailbox(guest->email, guest->firstName + " " + guest->lastName));
		}
		if (message.recipients().empty()) {
			log("info") << "Party " << party->name << " has no e-mail addresses, skipping";
		} else {
			message.setSubject(WString::tr("invitation.subject"));
			message.setBody(WString::tr("invitation.body").arg(party->name).arg(party->uuid));
			message.addHtmlBody(WString::tr("invitation.html").arg(party->name).arg(party->uuid));
			if (!client.send(message))
				log("error") << "Could not send e-mail to " << party->name;
			
			party.modify()->invited = WDateTime::currentDateTime();
			log("info") << "Sent invitation to " << party->name;
		}
	}
	log("info") << "All invitations were sent.";
	client.disconnect();
}

