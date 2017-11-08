#ifndef RSVPAPPLICATION_HPP_
#define RSVPAPPLICATION_HPP_

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>

namespace Wt {
  class WText;
}

class RsvpApplication : public Wt::WApplication
{
public:
  RsvpApplication(const Wt::WEnvironment& env);

private:
  std::vector<Wt::WLineEdit> *names_;
  Wt::WLineEdit *remarks_;
  Wt::WButton *submit_;
};

#endif // RSVPAPPLICATION_HPP_
