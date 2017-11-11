#include "rsvpApplication.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WServer.h>
using Wt::WServer;
using Wt::EntryPointType;
using Wt::WRun;

#include <Wt/WApplication.h>
using Wt::WApplication;

#include <Wt/WEnvironment.h>
using Wt::WEnvironment;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/WDate.h>
using Wt::WDate;

#include <Wt/WContainerWidget.h>
using Wt::WContainerWidget;

#include <Wt/WStackedWidget.h>
using Wt::WStackedWidget;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WText.h>
using Wt::WText;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WPushButton.h>
using Wt::WPushButton;

#include <Wt/WTemplate.h>
using Wt::WTemplate;

#include <Wt/Mail/Client.h>
using Wt::Mail::Client;

#include <Wt/Mail/Message.h>
using Wt::Mail::Message;
using Wt::Mail::RecipientType;

#include <Wt/Mail/Mailbox.h>
using Wt::Mail::Mailbox;

#include <Wt/Dbo/Dbo.h>
using Wt::Dbo::Transaction;

#include <Wt/Dbo/backend/Sqlite3.h>
using Wt::Dbo::backend::Sqlite3;

using std::make_unique;
using std::unique_ptr;
using std::move;

RsvpApplication::RsvpApplication(const WEnvironment& env, bool embedded)
  : WApplication(env)
{
	unique_ptr<Sqlite3> sqlite3(new Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(move(sqlite3));
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");
	
	const string *uuid = env.getParameter("uuid");
	if (!uuid) {
		log("error") << "No uuid parameter found";
		return;
	}
	if (*uuid == "Swf9TMCxFwkdrdahZLcE") {
		session_.createTables();
		Transaction transaction(session_);
		ptr<Party> party = session_.add(unique_ptr<Party>{new Party()});
		party.modify()->uuid = "test";
		party.modify()->email = "raf@localhost";
		ptr<Guest> guest = session_.add(unique_ptr<Guest>{new Guest()});
		guest.modify()->firstName = "Raf";
		guest.modify()->lastName = "Pauwels";
		guest.modify()->party = party;
		log("info") << "Database created.";
		return;
	} else if (*uuid == "m6xkGySLuNEf8StUaTzP") {
		log("info") << "E-mails sent.";
		return;
	}
	Transaction transaction(session_);
	party_ = session_.find<Party>().where("uuid = ?").bind(*uuid);
	if (!party_) {
		log("error") << "Party not found";
		return;
	}
	
	messageResourceBundle().use("resources");
	WContainerWidget *top;
	if (embedded) {
		auto topPtr = make_unique<WContainerWidget>();
		top = topPtr.get();
		setJavaScriptClass("rsvp");
		bindWidget(move(topPtr), "rsvp");
	} else {
		setTitle(WString::tr("title"));
		useStyleSheet("style.css");
		top = root();
	}
	auto names = top->addWidget(make_unique<WContainerWidget>());
	names->setList(true);
	remarks_ = top->addWidget(make_unique<WLineEdit>());
	remarks_->setInline(false);
	remarks_->setPlaceholderText(WString::tr("remarks"));
	auto submit = top->addWidget(make_unique<WPushButton>(WString::tr("submit")));
	submit->setInline(false);
	submit->clicked().connect(this, &RsvpApplication::submit);
	
	for (const ptr<Guest> &guest: party_->guests) {
		auto nameRow = names->addWidget(make_unique<WContainerWidget>());
		nameRow->addWidget(make_unique<WText>(guest->firstName + " " + guest->lastName));
		diet_ = nameRow->addWidget(make_unique<WComboBox>());
		diet_->addItem(WString::tr("absent"));
		diet_->addItem(WString::tr("herbivore"));
		diet_->addItem(WString::tr("carnivore"));
	}
}

void RsvpApplication::submit() {
	Transaction transaction(session_);
	party_.modify()->confirmed = WDate::currentDate();
	party_.modify()->remarks = remarks_->text().toUTF8();
	for (const ptr<Guest> &guest: party_->guests)
		guest.modify()->diet = static_cast<Diet>(diet_->currentIndex());
	Message message;
	message.setFrom(Mailbox("raf@localhost", "Raf Pauwels"));
	message.addRecipient(RecipientType::To, Mailbox("raf@localhost", "Raf Pauwels"));
	message.setSubject("Bevestiging");
	message.setBody("Bedankt om de registratie te bevestigen.");
	message.addHtmlBody("<p>Bedankt om de registratie te bevestigen.</p>");
	Client client;
	client.connect("localhost");
	client.send(message);
}

unique_ptr<WApplication> createApplication(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env, false);
}

unique_ptr<WApplication> createWidgetSet(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env, true);
}

int main(int argc, char **argv) {
	WServer server(argc, argv, WTHTTP_CONFIGURATION);
	server.addEntryPoint(EntryPointType::Application, createApplication);
	server.addEntryPoint(EntryPointType::WidgetSet, createWidgetSet, "/rsvp.js");
	server.run();
}

