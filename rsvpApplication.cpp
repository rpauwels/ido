#include "rsvpApplication.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WRun;

#include <Wt/WEnvironment.h>
using Wt::WEnvironment;

#include <Wt/WString.h>
using Wt::WString;

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
using Wt::Dbo::Session;
using Wt::Dbo::Transaction;

#include <Wt/Dbo/backend/Sqlite3.h>
using Wt::Dbo::backend::Sqlite3;

using std::make_unique;
using std::unique_ptr;
using std::move;

RsvpApplication::RsvpApplication(const WEnvironment& env)
  : WApplication(env)
{
	setTitle(WString::tr("title"));
	useStyleSheet("style.css");
	messageResourceBundle().use(appRoot() + "simplechat");
	
	auto t = make_unique<WTemplate>(WString::tr("template"));
	auto rows = t->bindWidget("rows", make_unique<WContainerWidget>());
	auto remarks = t->bindWidget("remarks", make_unique<WLineEdit>());
	auto submit = t->bindWidget("submit", make_unique<WPushButton>());	
	
  const string *uuid = env.getParameter("party");
	
	// TODO loop
	rows->setList(true);
	auto row = rows->addWidget(make_unique<WContainerWidget>());
	row->addWidget(make_unique<WText>("Raf Pauwels"));
	auto diet = row->addWidget(make_unique<WComboBox>());
	diet->addItem("kan er niet bij zijn");
	diet->addItem("is herbivoor");
	diet->addItem("is carnivoor");
	
	submit->clicked().connect(this, &RsvpApplication::submit);
}

void RsvpApplication::submit() {
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

int main(int argc, char **argv) {
	unique_ptr<Sqlite3> sqlite3(new Sqlite3(":memory:"));
	sqlite3->setProperty("show-queries", "true");
  Session session;
  session.setConnection(move(sqlite3));
  session.mapClass<Guest>("guest");
  session.mapClass<Party>("party");
  session.createTables();
  string uuid;
  {
  	Transaction transaction(session);
  	unique_ptr<Party> party{new Party()};
  	party->email = "raf@localhost";
  	uuid = party->uuid;
  	ptr<Party> partyPtr = session.add(move(party));
  }
  ptr<Guest> guest;
  {
  	Transaction transaction(session);
  	ptr<Party> party = session.find<Party>().where("uuid = ?").bind(uuid);
  	guest = session.add(unique_ptr<Guest>{new Guest()});
  	guest.modify()->party = party;
  }
  
	return WRun(argc, argv, [](const WEnvironment& env) {
		return make_unique<RsvpApplication>(env);
	});
}

