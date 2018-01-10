#include "rsvpApplication.hpp"
#include "adminApplication.hpp"
#include "calendarResource.hpp"
#include "guest.hpp"
#include "party.hpp"
#include "song.hpp"

#include <Wt/WServer.h>
using Wt::WServer;
using Wt::EntryPointType;
using Wt::WRun;

#include <Wt/WAnimation.h>
using Wt::WAnimation;
using Wt::AnimationEffect;
using Wt::TimingFunction;

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

#include <Wt/WLocalDateTime.h>
#include <Wt/WDateTime.h>
using Wt::WDateTime;

#include <Wt/WEnvironment.h>
using Wt::WEnvironment;

#include <Wt/WImage.h>
using Wt::WImage;

#include <Wt/WString.h>
using Wt::WString;

#include <Wt/WStackedWidget.h>
using Wt::WStackedWidget;

#include <Wt/WTemplate.h>
using Wt::WTemplate;
using Wt::TextFormat;

#include <Wt/WText.h>
using Wt::WText;

#include <Wt/WTextArea.h>
using Wt::WTextArea;

#include <Wt/WLabel.h>
using Wt::WLabel;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WLink.h>
using Wt::WLink;

#include <Wt/WLocale.h>
using Wt::WLocale;

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
using Wt::Dbo::collection;
using Wt::Dbo::ptr;

#include <Wt/Dbo/backend/Sqlite3.h>
using Wt::Dbo::backend::Sqlite3;

#include <Wt/Date/tz.h>
using date::current_zone;

#include <memory>
using std::make_unique;
using std::make_shared;
using std::unique_ptr;

#include <utility>
using std::move;
using std::make_pair;
using std::pair;

#include <string>
using std::to_string;

RsvpApplication::RsvpApplication(const WEnvironment& env)
  : WApplication(env) {
	useStyleSheet("animate.css/animate.min.css");
	messageResourceBundle().use("resources");
	unique_ptr<Sqlite3> sqlite3(new Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(move(sqlite3));
	session_.mapClass<Event>("event");
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");
	session_.mapClass<Song>("song");
	
	WLocale l = locale();
	l.setTimeZone(current_zone());
	setLocale(l);

	setTitle(WString::tr("pageTitle"));
	auto bootstrapTheme = make_shared<WBootstrapTheme>();
	bootstrapTheme->setVersion(BootstrapVersion::v3);
	bootstrapTheme->setResponsive(true);
	setTheme(bootstrapTheme);
	useStyleSheet("style.css");
	
	auto header = root()->addNew<WTemplate>(WString::tr("header"));
	header->addStyleClass("header");
	
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
	
	auto timelineWrapper = root()->addNew<WContainerWidget>();
	timelineWrapper->addStyleClass("timelineWrapper");
	auto timeline = timelineWrapper->addNew<WContainerWidget>();
	timeline->addStyleClass("timeline");
	auto ical = make_shared<CalendarResource>();
	for (const ptr<Event> &event: party_->events) {
		auto t = timeline->addNew<WTemplate>(WString::tr("event"));
		t->addStyleClass("event");
		event->fill(*t);
		ical->addEvent(*event);
	}
	auto calendar = timeline->addNew<WAnchor>(WLink(ical));
	calendar->addStyleClass("calendar-circle");
	calendar->setImage(make_unique<WImage>(WString::tr("calendarImg").toUTF8(), WString::tr("calendar")));
	calendar->image()->addStyleClass("calendar");

	rsvp_ = root()->addNew<WTemplate>(WString::tr("rsvp"));
	rsvp_->addStyleClass("rsvp");
	auto names = rsvp_->bindNew<WContainerWidget>("names");
	names->addStyleClass("names");
	names->setList(true);
	for (const ptr<Guest> &guest: party_->guests) {
		auto nameRow = names->addNew<WContainerWidget>();
		auto label = nameRow->addNew<WLabel>(guest->firstName + " " + guest->lastName);
		auto diet = nameRow->addNew<WComboBox>();
		label->setBuddy(diet);
		diet->addStyleClass("diet");
		diet->addItem(WString::tr("absent"));
		if (party_->inviteLevel == InviteLevel::Dessert) {
			diet->addItem(WString::tr("present"));
		} else {
			diet->addItem(WString::tr("herbivore"));
			diet->addItem(WString::tr("carnivore"));
			diet->addItem(WString::tr("pollotarian"));
		}
		diet->setCurrentIndex(static_cast<int>(guest->diet));
		diets_.push_back(diet);
	}
	songContainer_ = rsvp_->bindNew<WContainerWidget>("songs");
	for (const ptr<Song> &song: party_->songs)
		addSong(song->artist, song->title);
	addSong();
	remarks_ = rsvp_->bindNew<WTextArea>("remarks");
	remarks_->addStyleClass("remarks");
	remarks_->setColumns(35);
	remarks_->setPlaceholderText(WString::tr("remarks"));
	remarks_->setText(party_->remarks);
	setStatus();
	
	auto footer = root()->addNew<WText>(WString::tr("footer"));
	footer->addStyleClass("footer");
	party_.modify()->opened = WDateTime::currentDateTime();
	log("info") << "Party " << party_.id() << " (" << party_->name << ") loaded the page";
}

void RsvpApplication::addSong(const string& artist, const string& title) {
	if (songContainer_->count() > 6)
		return;
	auto songRow = songContainer_->addNew<WContainerWidget>();
	auto artistEdit = songRow->addNew<WLineEdit>(artist);
	artistEdit->setTextSize(15);
	artistEdit->setPlaceholderText(WString::tr("artist"));
	artistEdit->changed().connect(this, &RsvpApplication::songChanged);
	auto titleEdit = songRow->addNew<WLineEdit>(title);
	titleEdit->setTextSize(15);
	titleEdit->setPlaceholderText(WString::tr("title"));
	titleEdit->changed().connect(this, &RsvpApplication::songChanged);
	songRow->animateShow(WAnimation(AnimationEffect::SlideInFromTop | AnimationEffect::Fade, TimingFunction::EaseOut, 250));
	songs_.push_back(make_pair(artistEdit, titleEdit));
}

void RsvpApplication::setStatus() {
	WString submitText;
	WString statusText;
	submit_ = rsvp_->bindNew<WPushButton>("submit", WString::tr("submit"));
	if (party_->confirmed.isNull())
		statusText = WString::tr("status.notSubmitted");
	else
		statusText = WString::tr("status.submitted").arg(party_->confirmed.toLocalTime().toString("yyyy-MM-dd HH:mm:ss"));
	auto status = rsvp_->bindNew<WText>("status", statusText);
	status->addStyleClass("alert");
	submit_->clicked().connect(this, &RsvpApplication::submit);
	rsvp_->setDisabled(!party_->confirmed.isNull());
}

void RsvpApplication::songChanged() {
	auto it = songs_.begin();
	for (int i = 0; it < songs_.end() - 1; ++i) {
		if (it->first->text().empty() && it->second->text().empty()) {
			songContainer_->removeWidget(songContainer_->widget(i));
			songs_.erase(it);
		} else {
			++it;
		}
	}
	if (!it->first->text().empty() || !it->second->text().empty())
		addSong();
}

void RsvpApplication::submit() {
	submit_->setEnabled(false);
	Transaction transaction(session_);
	party_.reread();
	party_.modify()->confirmed = WDateTime::currentDateTime();
	party_.modify()->remarks = remarks_->text().toUTF8();
	int i = 0;
	for (const ptr<Guest> &guest: party_->guests)
		guest.modify()->diet = static_cast<Diet>(diets_[i++]->currentIndex());
	party_.modify()->songs.clear();
	int j = 0;
	for (const pair<WLineEdit*, WLineEdit*> &pair: songs_) {
		if (!pair.first->text().empty() || !pair.second->text().empty()) {
			unique_ptr<Song> song{new Song()};
			song->artist = pair.first->text().toUTF8();
			song->title = pair.second->text().toUTF8();
			song->order = j++;
			song->party = party_;
			session_.add(move(song));
		}
	}
	Message message;
	message.setFrom(Mailbox(WString::tr("fromAddress").toUTF8(), WString::tr("fromName")));
	for (const ptr<Guest> &guest: party_->guests) {
		if (!guest->email.empty())
			message.addRecipient(RecipientType::To, Mailbox(guest->email, guest->firstName + " " + guest->lastName));
	}
	message.setSubject(WString::tr("confirmation.subject"));
	message.setBody(WString::tr("confirmation.body").arg(party_->name).arg(party_->uuid));
	message.addHtmlBody(WString::tr("confirmation.html").arg(party_->name).arg(party_->uuid));
	Client client;
	if (!client.connect())
		log("error") << "Could not connect to SMTP server";
	if (!client.send(message))
		log("error") << "Could not send confirmation to " << party_->name;
	client.disconnect();
	setStatus();
}

unique_ptr<WApplication> createApplication(const WEnvironment& env) {
	return make_unique<RsvpApplication>(env);
}

unique_ptr<WApplication> createAdminApplication(const WEnvironment& env) {
	return make_unique<AdminApplication>(env);
}

int main(int argc, char **argv) {
	WServer server(argc, argv, WTHTTP_CONFIGURATION);
	server.addEntryPoint(EntryPointType::Application, createApplication);
	server.addEntryPoint(EntryPointType::Application, createAdminApplication, "/admin");
	server.run();
}
