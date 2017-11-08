#include "rsvpApplication.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;
using Wt::WRun;

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

using std::unique_ptr;
using std::move;

RsvpApplication::RsvpApplication(const WEnvironment& env)
  : WApplication(env)
{
	setTitle("raf + véronique");
	WTemplate *result = new WTemplate("index.html", root());
	WStackedWidget *rows = new WStackedWidget();
	
	// TODO loop
	WContainerWidget *row = new WContainerWidget(&rows);
	WText *name = new WText("Raf Pauwels", &row);
	WText *participates = new WText(" zal er zijn en is ", &row);
	WComboBox *diet = new WComboBox(&row);
	diet->addItem("kan er niet bij zijn");
	diet->addItem("is herbivoor");
	diet->addItem("is carnivoor");
	
	WLineEdit *remarks = new WLineEdit(&rows);
	WPushButton *submit = new WPushButton(&rows);
	submit->clicked().connect(this, &FormExample::submit);
	
	result->bindWidget("rows", rows);
	result->bindWidget("submitButton", submitButton);
  Form *form = new Form(this);
}

void FormExample::submit() {
	Message message;
	message.setFrom(Mailbox("raf@localhost", "Raf Pauwels");
	message.addRecipient(RecipientType::To, Mailbox("raf@localhost", "Raf Pauwels");
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

