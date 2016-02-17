/**
 * This source code is governed by by the boost v1 licensing terms and conditions.
 * Copyright 2016 Nicholas A. Smith (nickrmc83@gmail.com)
 */
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
    void operator()(const T &ev)
    {
      std::cout << "Received event of value " << ev.event_value << std::endl;
    }
};

typedef subscriber<int_event> int_subscriber;
typedef subscriber<string_event> string_subscriber;

int main(int argc, char **argv)
{
  int result = 1;
  asynchro_event_bus bus;
  int_subscriber *ptr = new int_subscriber();
  std::shared_ptr<event_subscriber<event<int>>> isubscriber1(ptr);
  int_subscriber isubscriber2;
  string_subscriber ssubscriber1;
  string_subscriber ssubscriber2;
  bus.subscribe(&isubscriber2)
    .subscribe(isubscriber1)
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
    .publish(sevent)
    .publish(true);
  try
  {
    bus.subscribe(isubscriber1);
  }
  catch(const already_subscribed_exception &ex)
  {
    result = 0;
    std::cout << "Caught expected exception:" << ex.what() << std::endl;
  }
  bus.unsubscribe(isubscriber1);
  bus.subscribe(isubscriber1);
  bus.unsubscribe(isubscriber1);
  bus.unsubscribe(&isubscriber2);
  return result;
}
