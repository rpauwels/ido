#include "rsvpApplication.hpp"
#include "adminApplication.hpp"
#include "calendarResource.hpp"
#include "guest.hpp"
#include "party.hpp"

#include <Wt/WServer.h>
using Wt::WServer;
using Wt::EntryPointType;
using Wt::WRun;

#include <Wt/WApplication.h>
using Wt::WApplication;

#include <Wt/WAnchor.h>
using Wt::WAnchor;

#include <Wt/WBootstrapTheme.h>
using Wt::WBootstrapTheme;
using Wt::BootstrapVersion;

#include <Wt/WComboBox.h>
using Wt::WComboBox;

#include <Wt/WContainerWidget.h>
using Wt::WContainerWidget;

#include <Wt/WDate.h>
using Wt::WDate;

#include <Wt/WEnvironment.h>
using Wt::WEnvironment;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/WStackedWidget.h>
using Wt::WStackedWidget;

#include <Wt/WTemplate.h>
using Wt::WTemplate;
using Wt::TextFormat;

#include <Wt/WText.h>
using Wt::WText;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WLink.h>
using Wt::WLink;

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
using std::make_shared;
using std::move;
using std::unique_ptr;

RsvpApplication::RsvpApplication(const WEnvironment& env, bool embedded)
  : WApplication(env) {
	messageResourceBundle().use("resources");
	unique_ptr<Sqlite3> sqlite3(new Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(move(sqlite3));
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");

	WContainerWidget *top;
	WContainerWidget *plan;
	if (embedded) {
		auto planPtr = make_unique<WContainerWidget>();
		plan = planPtr.get();
		bindWidget(move(planPtr), "plan");
	
		setJavaScriptClass("rsvp");
		auto topPtr = make_unique<WContainerWidget>();
		top = topPtr.get();
		bindWidget(move(topPtr), "rsvp");
	} else {
		setTitle(WString::tr("title"));
		useStyleSheet("bootstrap/css/bootstrap.css");
		useStyleSheet("style.css");
		auto bootstrapTheme = make_shared<WBootstrapTheme>();
		bootstrapTheme->setVersion(BootstrapVersion::v3);
		bootstrapTheme->setResponsive(true);
		setTheme(bootstrapTheme);
		top = root();
		plan = root();
		root()->addWidget(make_unique<WTemplate>(WString::tr("header")));
	}
	
	const string *uuid = env.getParameter("uuid");
	if (!uuid) {
		log("error") << "Uuid parameter absent";
		return;
	}
	Transaction transaction(session_);
	party_ = session_.find<Party>().where("uuid = ?").bind(*uuid);
	if (!party_) {
		log("error") << "Uuid value " << *uuid << " not found";
		return;
	}
	
	auto t = plan->addWidget(make_unique<WTemplate>(WString::tr("plan")));
	t->setCondition("if-meal", party_->inviteLevel >= InviteLevel::Meal);
	t->setCondition("if-full", party_->inviteLevel == InviteLevel::Full);
	auto ical = WLink(make_shared<CalendarResource>(party_->inviteLevel));
	plan->addWidget(make_unique<WAnchor>(ical, WString::tr("addToCalendar")));

	auto names = top->addWidget(make_unique<WContainerWidget>());
	names->setList(true);
	remarks_ = top->addWidget(make_unique<WLineEdit>());
	remarks_->setInline(false);
	remarks_->setTextSize(20);
	remarks_->setPlaceholderText(WString::tr("remarks"));
	remarks_->setText(party_->remarks);
	WString buttonText;
	if (party_->confirmed.isNull()) {
		submit_ = top->addWidget(make_unique<WPushButton>(WString::tr("submit")));
		status_ = top->addWidget(make_unique<WText>(WString::tr("status.notSubmitted")));
	} else {
		submit_ = top->addWidget(make_unique<WPushButton>(WString::tr("change")));
		status_ = top->addWidget(make_unique<WText>(WString::tr("status.submitted")));
	}
	submit_->setInline(false);
	submit_->clicked().connect(this, &RsvpApplication::submit);
	for (const ptr<Guest> &guest: party_->guests) {
		auto nameRow = names->addWidget(make_unique<WContainerWidget>());
		nameRow->addWidget(make_unique<WText>(guest->firstName + " " + guest->lastName));
		auto diet = nameRow->addWidget(make_unique<WComboBox>());
		diet->addStyleClass("diet");
		diet->addItem(WString::tr("absent"));
		diet->addItem(WString::tr("herbivore"));
		diet->addItem(WString::tr("carnivore"));
		diet->setCurrentIndex(static_cast<int>(guest->diet));
		diets_.push_back(diet);
	}
	party_.modify()->opened = WDate::currentDate();
}

void RsvpApplication::submit() {
	submit_->setEnabled(false);
	Transaction transaction(session_);
	party_.modify()->confirmed = WDate::currentDate();
	party_.modify()->remarks = remarks_->text().toUTF8();
	int i = 0;
	for (const ptr<Guest> &guest: party_->guests)
		guest.modify()->diet = static_cast<Diet>(diets_[i++]->currentIndex());
	ptr<Guest> guest = party_->guests.front();
	Message message;
	message.setFrom(Mailbox(WString::tr("fromAddress").toUTF8(), WString::tr("fromName")));
	message.addRecipient(RecipientType::To, Mailbox(party_->email, guest->firstName + " " + guest->lastName));
	message.setSubject(WString::tr("confirmation.subject"));
	message.setBody(WString::tr("confirmation.body").arg(party_->name));
	message.addHtmlBody(WString::tr("confirmation.html").arg(party_->name));
	client_.connect();
	client_.send(message);
	status_->setText(WString::tr("status.submitted"));
	submit_->setText(WString::tr("change"));
	submit_->setEnabled(true);
}

unique_ptr<WApplication> createApplication(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env, false);
}

unique_ptr<WApplication> createWidgetSet(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env, true);
}

unique_ptr<WApplication> createAdminApplication(const WEnvironment& env) {
	return make_unique<AdminApplication>(env);
}

int main(int argc, char **argv) {
	WServer server(argc, argv, WTHTTP_CONFIGURATION);
	server.addEntryPoint(EntryPointType::Application, createApplication);
	server.addEntryPoint(EntryPointType::Application, createAdminApplication, "/admin");
	server.addEntryPoint(EntryPointType::WidgetSet, createWidgetSet, "/rsvp.js");
	server.run();
}
