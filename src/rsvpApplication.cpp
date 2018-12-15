/*
	ido: an RSVP web application for weddings
	Copyright (C) 2017  Raf Pauwels

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, see <https://www.gnu.org/licenses/>.
*/

#include "rsvpApplication.hpp"
#include "adminApplication.hpp"
#include "calendarResource.hpp"
#include "guest.hpp"
#include "party.hpp"
#include "song.hpp"

#include <Wt/WServer.h>
#include <Wt/WAnimation.h>
#include <Wt/WApplication.h>
#include <Wt/WAnchor.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLocalDateTime.h>
#include <Wt/WDateTime.h>
#include <Wt/WEnvironment.h>
#include <Wt/WImage.h>
#include <Wt/WString.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLink.h>
#include <Wt/WLocale.h>
#include <Wt/WPushButton.h>
#include <Wt/Mail/Client.h>
#include <Wt/Mail/Message.h>
#include <Wt/Mail/Mailbox.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/Date/tz.h>

#include <memory>
#include <utility>
#include <string>

RsvpApplication::RsvpApplication(const Wt::WEnvironment& env)
  : Wt::WApplication(env) {
	useStyleSheet("/animate.min.css");
	messageResourceBundle().use(appRoot() + "resources");
	std::unique_ptr<Wt::Dbo::backend::Sqlite3> sqlite3(new Wt::Dbo::backend::Sqlite3("rsvp.db"));
	sqlite3->setProperty("show-queries", "true");
	session_.setConnection(std::move(sqlite3));
	session_.mapClass<Event>("event");
	session_.mapClass<Guest>("guest");
	session_.mapClass<Party>("party");
	session_.mapClass<Song>("song");
	
	Wt::WLocale l = locale();
	l.setTimeZone(date::current_zone());
	setLocale(l);

	setTitle(Wt::WString::tr("pageTitle"));
	auto bootstrapTheme = std::make_shared<Wt::WBootstrapTheme>();
	bootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
	bootstrapTheme->setResponsive(true);
	setTheme(bootstrapTheme);
	useStyleSheet("style.css");
	
	auto header = root()->addNew<Wt::WTemplate>(Wt::WString::tr("header"));
	header->addStyleClass("header");
	
	const std::string *uuid = env.getParameter("uuid");
	if (!uuid) {
		Wt::log("error") << "Uuid parameter absent";
		return;
	}
	Wt::Dbo::Transaction transaction(session_);
	party_ = session_.find<Party>().where("uuid = ?").bind(*uuid);
	if (!party_) {
		Wt::log("error") << "Uuid value " << *uuid << " not found";
		return;
	}
	
	auto timelineWrapper = root()->addNew<Wt::WContainerWidget>();
	timelineWrapper->addStyleClass("timelineWrapper");
	auto timeline = timelineWrapper->addNew<Wt::WContainerWidget>();
	timeline->addStyleClass("timeline");
	auto ical = std::make_shared<CalendarResource>();
	for (auto event: party_->events) {
		auto t = timeline->addNew<Wt::WTemplate>(Wt::WString::tr("event"));
		t->addStyleClass("event");
		event->fill(*t);
		ical->addEvent(*event);
	}
	auto calendar = timeline->addNew<Wt::WAnchor>(Wt::WLink(ical));
	calendar->addStyleClass("calendar-circle");
	calendar->setImage(std::make_unique<Wt::WImage>(Wt::WString::tr("calendarImg").toUTF8(), 
	                                                Wt::WString::tr("calendar")));
	calendar->image()->addStyleClass("calendar");

	rsvp_ = root()->addNew<Wt::WTemplate>(Wt::WString::tr("rsvp"));
	rsvp_->addStyleClass("rsvp");
	auto names = rsvp_->bindNew<Wt::WContainerWidget>("names");
	names->addStyleClass("names");
	names->setList(true);
	for (auto guest: party_->guests) {
		auto nameRow = names->addNew<Wt::WContainerWidget>();
		auto label = nameRow->addNew<Wt::WLabel>(guest->firstName + " " + guest->lastName);
		auto diet = nameRow->addNew<Wt::WComboBox>();
		label->setBuddy(diet);
		diet->addStyleClass("diet");
		diet->addItem(Wt::WString::tr("absent"));
		if (party_->inviteLevel == InviteLevel::Dessert) {
			diet->addItem(Wt::WString::tr("present"));
		} else {
			diet->addItem(Wt::WString::tr("herbivore"));
			diet->addItem(Wt::WString::tr("carnivore"));
			diet->addItem(Wt::WString::tr("pollotarian"));
		}
		diet->setCurrentIndex(static_cast<int>(guest->diet));
		diets_.push_back(diet);
	}
	songContainer_ = rsvp_->bindNew<Wt::WContainerWidget>("songs");
	for (auto song: party_->songs)
		addSong(song->artist, song->title);
	addSong();
	remarks_ = rsvp_->bindNew<Wt::WTextArea>("remarks");
	remarks_->addStyleClass("remarks");
	remarks_->setColumns(35);
	remarks_->setPlaceholderText(Wt::WString::tr("remarks"));
	remarks_->setText(party_->remarks);
	setStatus();
	
	auto footer = root()->addNew<Wt::WText>(Wt::WString::tr("footer"));
	footer->addStyleClass("footer");
	party_.modify()->opened = Wt::WDateTime::currentDateTime();
	Wt::log("info") << "Party " << party_.id() << " (" << party_->name << ") loaded the page";
}

void RsvpApplication::addSong(const std::string& artist, const std::string& title) {
	if (songContainer_->count() > 6)
		return;
	auto songRow = songContainer_->addNew<Wt::WContainerWidget>();
	auto artistEdit = songRow->addNew<Wt::WLineEdit>(artist);
	artistEdit->setTextSize(15);
	artistEdit->setPlaceholderText(Wt::WString::tr("artist"));
	artistEdit->changed().connect(this, &RsvpApplication::songChanged);
	auto titleEdit = songRow->addNew<Wt::WLineEdit>(title);
	titleEdit->setTextSize(15);
	titleEdit->setPlaceholderText(Wt::WString::tr("title"));
	titleEdit->changed().connect(this, &RsvpApplication::songChanged);
	songRow->animateShow(Wt::WAnimation(Wt::AnimationEffect::SlideInFromTop | Wt::AnimationEffect::Fade, 
	                                    Wt::TimingFunction::EaseOut, 250));
	songs_.push_back(std::make_pair(artistEdit, titleEdit));
}

void RsvpApplication::setStatus() {
	Wt::WString submitText;
	Wt::WString statusText;
	submit_ = rsvp_->bindNew<Wt::WPushButton>("submit", Wt::WString::tr("submit"));
	if (party_->confirmed.isNull()) {
		statusText = Wt::WString::tr("status.notSubmitted");
	} else {
		std::string places = session_.query<std::string>("select group_concat(place, ' en ') \
			from (select distinct place from guest where party_id = ? and diet != 0 and place != '')").bind(party_.id());
		if (places.empty())
			statusText = Wt::WString::tr("status.submitted").arg(party_->confirmed.toLocalTime()
			                                                                      .toString("yyyy-MM-dd HH:mm:ss"));
		else
			statusText = Wt::WString::tr("status.places").arg(places);
	}
	auto status = rsvp_->bindNew<Wt::WText>("status", statusText);
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
	Wt::Dbo::Transaction transaction(session_);
	party_.reread();
	party_.modify()->confirmed = Wt::WDateTime::currentDateTime();
	party_.modify()->remarks = remarks_->text().toUTF8();
	int i = 0;
	for (auto guest: party_->guests)
		guest.modify()->diet = static_cast<Diet>(diets_[i++]->currentIndex());
	party_.modify()->songs.clear();
	int j = 0;
	for (auto pair: songs_) {
		if (!pair.first->text().empty() || !pair.second->text().empty()) {
			std::unique_ptr<Song> song{new Song()};
			song->artist = pair.first->text().toUTF8();
			song->title = pair.second->text().toUTF8();
			song->order = j++;
			song->party = party_;
			session_.add(std::move(song));
		}
	}
	Wt::Mail::Message message;
	message.setFrom(Wt::Mail::Mailbox(Wt::WString::tr("fromAddress").toUTF8(), Wt::WString::tr("fromName")));
	for (auto guest: party_->guests) {
		if (!guest->email.empty())
			message.addRecipient(Wt::Mail::RecipientType::To, 
			                     Wt::Mail::Mailbox(guest->email, guest->firstName + " " + guest->lastName));
	}
	message.setSubject(Wt::WString::tr("confirmation.subject"));
	message.setBody(Wt::WString::tr("confirmation.body").arg(party_->name).arg(party_->uuid));
	message.addHtmlBody(Wt::WString::tr("confirmation.html").arg(party_->name).arg(party_->uuid));
	Wt::Mail::Client client;
	if (!client.connect())
		Wt::log("error") << "Could not connect to SMTP server";
	if (!client.send(message))
		Wt::log("error") << "Could not send confirmation to " << party_->name;
	client.disconnect();
	setStatus();
}

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
	return std::make_unique<RsvpApplication>(env);
}

std::unique_ptr<Wt::WApplication> createAdminApplication(const Wt::WEnvironment& env) {
	return std::make_unique<AdminApplication>(env);
}

int main(int argc, char **argv) {
	enum Error {none, exception, serverException};
	try {
		Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);
		server.addEntryPoint(Wt::EntryPointType::Application, createApplication);
		server.addEntryPoint(Wt::EntryPointType::Application, createAdminApplication, "/admin");
		server.run();
	} catch (const Wt::WServer::Exception& e) {
		std::cerr << e.what() << std::endl;
		return Error::serverException;
	} catch (const std::exception &e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return Error::exception;
	}
	return Error::none;
}
