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
using std::move;

RsvpApplication::RsvpApplication(const WEnvironment& env, bool embedded)
  : WApplication(env)
{
	messageResourceBundle().use("resources");
	client_.connect("localhost");
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
		collection< ptr<Party> > parties = session_.find<Party>();
		for (const ptr<Party> &party: parties) {
			ptr<Guest> guest = party->guests.front();
			Message message;
			message.setFrom(Mailbox("raf@localhost", WString::tr("fromName")));
			message.addRecipient(RecipientType::To, Mailbox(party->email, guest->firstName + " " + guest->lastName));
			message.setSubject(WString::tr("invitation.subject"));
			message.setBody(WString::tr("invitation.body"));
			message.addHtmlBody(WString::tr("invitation.html"));
			client_.send(message);
			log("info") << "Sent invitation to " + party->email;
		}
		log("info") << "All invitations were sent.";
		return;
	}
	Transaction transaction(session_);
	party_ = session_.find<Party>().where("uuid = ?").bind(*uuid);
	if (!party_) {
		log("error") << "Party not found";
		return;
	}

	WContainerWidget *top;
	if (embedded) {
		setJavaScriptClass("rsvp");
		auto topPtr = make_unique<WContainerWidget>();
		top = topPtr.get();
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
	remarks_->setText(party_->remarks);
	WString buttonText;
	if (party_->confirmed.isNull()) {
		buttonText = WString::tr("submit");
	} else {
		top->addWidget(make_unique<WText>(WString::tr("alreadySubmitted")));
		buttonText = WString::tr("change");
	}
	submit_ = top->addWidget(make_unique<WPushButton>(buttonText));
	submit_->setInline(false);
	submit_->clicked().connect(this, &RsvpApplication::submit);

	for (const ptr<Guest> &guest: party_->guests) {
		auto nameRow = names->addWidget(make_unique<WContainerWidget>());
		nameRow->addWidget(make_unique<WText>(guest->firstName + " " + guest->lastName));
		diet_ = nameRow->addWidget(make_unique<WComboBox>());
		diet_->addItem(WString::tr("absent"));
		diet_->addItem(WString::tr("herbivore"));
		diet_->addItem(WString::tr("carnivore"));
		diet_->setCurrentIndex(static_cast<int>(guest->diet));
	}
}

void RsvpApplication::submit() {
	submit_->setEnabled(false);
	Transaction transaction(session_);
	party_.modify()->confirmed = WDate::currentDate();
	party_.modify()->remarks = remarks_->text().toUTF8();
	for (const ptr<Guest> &guest: party_->guests)
		guest.modify()->diet = static_cast<Diet>(diet_->currentIndex());
	ptr<Guest> guest = party_->guests.front();
	Message message;
	message.setFrom(Mailbox("raf@localhost", WString::tr("fromName")));
	message.addRecipient(RecipientType::To, Mailbox(party_->email, guest->firstName + " " + guest->lastName));
	message.setSubject(WString::tr("confirmation.subject"));
	message.setBody(WString::tr("confirmation.body"));
	message.addHtmlBody(WString::tr("confirmation.html"));
	client_.send(message);
	submit_->setText(WString::tr("change"));
	submit_->setEnabled(true);
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

