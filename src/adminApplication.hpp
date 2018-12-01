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

#ifndef ADMINAPPLICATION_HPP_
#define ADMINAPPLICATION_HPP_

#include <Wt/WApplication.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/Dbo/Dbo.h>

/*! \brief Administration interface for sending e-mails.
 *
 * A bare administration interface, allowing to send e-mails 
 * to selected groups.
 */
class AdminApplication : public Wt::WApplication {
public:
	//! Connects and maps database and creates the UI.
	AdminApplication(const Wt::WEnvironment& env);

private:
	Wt::Dbo::Session session_;
	Wt::WPushButton *create_;
	Wt::WComboBox *inviteLevel_;
	Wt::WPushButton *invite_;
	void create();
	void invite();
	void status(const std::string& status);
	void sendUpdates();
};

#endif // ADMINAPPLICATION_HPP_
