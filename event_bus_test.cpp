#include "event_bus.hpp"
#include <iostream>
#include <string>

using namespace events;

template<typename T>
struct event
{
  public:
    T event_value;
};

typedef event<int> int_event;
typedef event<std::string> string_event;

template<typename T>
class subscriber : public event_subscriber<T>
{
  public:
    void handle_event(const T &ev)
    {
      std::cout << "Received event of value " << ev.event_value << std::endl;
    }
};

typedef subscriber<int_event> int_subscriber;
typedef subscriber<string_event> string_subscriber;

int main(int argc, char **argv)
{
  asynchro_event_bus bus;
  int_subscriber isubscriber;
  string_subscriber ssubscriber1;
  string_subscriber ssubscriber2;
  bus.subscribe(&isubscriber)
    .subscribe(&ssubscriber1)
    .subscribe(&ssubscriber2);
  int_event ievent;
  ievent.event_value = 101;
  string_event sevent;
  sevent.event_value = "Nick";
  bus.publish(ievent)
    .publish(sevent)
    .publish(sevent)
    .publish(sevent)
    .publish(sevent)
    .publish(sevent)
    .publish(sevent)
    .publish(sevent);
  bus.unsubscribe(&isubscriber);
  return 0;
}
