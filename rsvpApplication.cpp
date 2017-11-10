#include "rsvpApplication.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WServer.h>
using Wt::WServer;
using Wt::EntryPointType;

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

RsvpApplication::RsvpApplication(const WEnvironment& env, bool embedded)
  : WApplication(env)
{
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
	//top->setInline(false);
	auto names = top->addWidget(make_unique<WContainerWidget>());
	auto remarks = top->addWidget(make_unique<WLineEdit>());
	remarks->setInline(false);
	auto submit = top->addWidget(make_unique<WPushButton>(WString::tr("submit")));
	submit->setInline(false);
  const string *uuid = env.getParameter("uuid");
	
	// TODO loop
	names->setList(true);
	auto nameRow = names->addWidget(make_unique<WContaibnerWidget>());
	nameRow->addWidget(make_unique<WText>("Raf Pauwels"));
	auto diet = nameRow->addWidget(make_unique<WComboBox>());
	diet->addItem(WString::tr("absent"));
	diet->addItem(WString::tr("herbivore"));
	diet->addItem(WString::tr("carnivore"));
	
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

unique_ptr<WApplication> createApplication(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env, false);
}

unique_ptr<WApplication> createWidgetSet(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env, true);
}

int main(int argc, char **argv) {
	unique_ptr<Sqlite3> sqlite3(new Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
  Session session;
  session.setConnection(move(sqlite3));
  session.mapClass<Guest>("guest");
  session.mapClass<Party>("party");
  session.createTables();
  string uuid;
  {
  	Transaction transaction(session);
  	ptr<Party> party = session.add(unique_ptr<Party>{new Party()});
  	party.modify()->email = "raf@localhost";
  	uuid = party->uuid;
  }
  ptr<Guest> guest;
  {
  	Transaction transaction(session);
  	guest = session.add(unique_ptr<Guest>{new Guest()});
  	guest.modify()->party = session.find<Party>().where("uuid = ?").bind(uuid);
  }
  
  WServer server(argc, argv, WTHTTP_CONFIGURATION);
  server.addEntryPoint(EntryPointType::Application, createApplication);
  server.addEntryPoint(EntryPointType::WidgetSet, createWidgetSet, "/rsvp.js");
  server.run();
}

