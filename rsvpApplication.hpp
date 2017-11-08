#ifndef RSVPAPPLICATION_HPP_
#define RSVPAPPLICATION_HPP_

#include <Wt/WApplication.h>
using Wt::WApplication;
using Wt::WEnvironment;

#include <Wt/WLineEdit.h>
using Wt::WLineEdit;

#include <Wt/WPushButton.h>
using Wt::WPushButton;

#include <vector>
using std::vector;

namespace Wt {
  class WText;
}

class RsvpApplication : public WApplication
{
public:
  RsvpApplication(const WEnvironment& env);

private:
  vector<WLineEdit> *names_;
  WLineEdit *remarks_;
  WPushButton *submit_;
};

#endif // RSVPAPPLICATION_HPP_
