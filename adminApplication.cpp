#include "adminApplication.hpp"
#include "guest.hpp"
#include "party.hpp"
#include "song.hpp"

#include <Wt/WApplication.h>
#include <Wt/WComboBox.h>
#include <Wt/WDateTime.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLogger.h>
#include <Wt/WPushButton.h>
#include <Wt/WString.h>
#include <Wt/Mail/Client.h>
#include <Wt/Mail/Message.h>
#include <Wt/Mail/Mailbox.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>

#include <string>
#include <tuple>

AdminApplication::AdminApplication(const Wt::WEnvironment& env)
  : Wt::WApplication(env) {
	messageResourceBundle().use("resources");
	std::unique_ptr<Wt::Dbo::backend::Sqlite3> sqlite3(new Wt::Dbo::backend::Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(move(sqlite3));
	session_.mapClass<Event>("event");
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");
	session_.mapClass<Song>("song");
	
	create_ = root()->addNew<Wt::WPushButton>(Wt::WString::tr("create"));
	create_->setInline(false);
	create_->clicked().connect(this, &AdminApplication::create);
	
	auto inviteGroup = root()->addNew<Wt::WGroupBox>();
	inviteLevel_ = inviteGroup->addNew<Wt::WComboBox>();
	inviteLevel_->addItem(Wt::WString::tr("dessert"));
	inviteLevel_->addItem(Wt::WString::tr("meal"));
	inviteLevel_->addItem(Wt::WString::tr("full"));
	invite_ = inviteGroup->addNew<Wt::WPushButton>(Wt::WString::tr("invite"));
	invite_->setInline(false);
	invite_->clicked().connect(this, &AdminApplication::invite);
	
	auto sendUpdates = root()->addNew<Wt::WPushButton>(Wt::WString::tr("sendUpdates"));
	sendUpdates->setInline(false);
	sendUpdates->clicked().connect(this, &AdminApplication::sendUpdates);
}

void AdminApplication::create() {
	session_.createTables();
	Wt::log("info") << "Database created.";
}

void AdminApplication::invite() {
	Wt::Dbo::Transaction transaction(session_);
	Wt::Dbo::collection< Wt::Dbo::ptr<Party> > parties = session_.find<Party>().where("inviteLevel = ?")
	                                                             .bind(inviteLevel_->currentIndex())
	                                                             .where("invited is null");
	Wt::Mail::Client client;
	if (!client.connect()) {
		Wt::log("error") << "Could not connect to SMTP server";
		return;
	}
	for (auto party: parties) {
		Wt::Mail::Message message;
		message.setFrom(Wt::Mail::Mailbox(Wt::WString::tr("fromAddress").toUTF8(), Wt::WString::tr("fromName")));
		for (auto guest: party->guests) {
			if (!guest->email.empty())
				message.addRecipient(Wt::Mail::RecipientType::To, 
				                     Wt::Mail::Mailbox(guest->email, guest->firstName + " " + guest->lastName));
		}
		message.setSubject(Wt::WString::tr("invitation.subject"));
		message.setBody(Wt::WString::tr("invitation.body").arg(party->name).arg(party->uuid));
		message.addHtmlBody(Wt::WString::tr("invitation.html").arg(party->name).arg(party->uuid));
		if (message.recipients().empty()) {
			Wt::log("error") << "Party " << party->name << " has no e-mail addresses, skipping";
		} else if (!client.send(message)) {
			Wt::log("error") << "Could not send e-mail to " << party->name;
		} else {
			party.modify()->invited = Wt::WDateTime::currentDateTime();
			Wt::log("info") << "Sent invitation to " << party->name;
		}
	}
	Wt::log("info") << "All invitations were sent.";
	client.disconnect();
}

void AdminApplication::sendUpdates() {
	Wt::Dbo::Transaction transaction(session_);
	Wt::Dbo::collection< std::tuple< Wt::Dbo::ptr<Party>, std::string > > results = 
		session_.query< std::tuple< Wt::Dbo::ptr<Party>, std::string > >("\
		select Party, group_concat(place, ' en ') \
		from (select distinct party_id, place from guest where diet != 0 and place != '') \
		join party Party on (party_id = party.id) where confirmed is not null group by party_id");
	Wt::Mail::Client client;
	if (!client.connect()) {
		Wt::log("error") << "Could not connect to SMTP server";
		return;
	}
	for (auto &result: results) {
		Wt::Mail::Message message;
		message.setFrom(Wt::Mail::Mailbox(Wt::WString::tr("fromAddress").toUTF8(), Wt::WString::tr("fromName")));
		auto party = std::get<0>(result);
		for (auto guest: party->guests) {
			if (!guest->email.empty())
				message.addRecipient(Wt::Mail::RecipientType::To, 
				                     Wt::Mail::Mailbox(guest->email, guest->firstName + " " + guest->lastName));
		}
		message.setSubject(Wt::WString::tr("update.subject"));
		message.setBody(Wt::WString::tr("update.body").arg(party->name).arg(std::get<1>(result)).arg(party->uuid));
		message.addHtmlBody(Wt::WString::tr("update.html").arg(party->name).arg(std::get<1>(result)).arg(party->uuid));
		if (message.recipients().empty()) {
			Wt::log("error") << "Party " << party->name << " has no e-mail addresses, skipping";
		} else if (!client.send(message)) {
			Wt::log("error") << "Could not send e-mail to " << party->name;
		} else {
			Wt::log("info") << "Sent update to " << party->name << " (" << std::get<1>(result) << ")";
		}
	}
	Wt::log("info") << "All updates were sent.";
	client.disconnect();
}
