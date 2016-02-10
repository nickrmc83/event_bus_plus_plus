#include <vector>
#include <map>
#include <string>
#include <typeinfo>
#include <memory>
#include <typeindex>
#include <iostream>
#include <type_traits>
#include <future>

namespace events
{
template<typename event>
class event_subscriber
{
  public:
    virtual void handle_event(const event &ev) = 0;
};

class synchronous_event_publish_strategy
{
  public:
    template<typename event>
      void operator()(const event &ev,
          const std::vector< event_subscriber<event> * > &subscribers)
      {
        for(auto subscriber = subscribers.begin();
            subscriber != subscribers.end();
            subscriber++)
        {
          (*subscriber)->handle_event(ev);
        }
      }
};

class asynchronous_event_publish_strategy
{
  private:
    synchronous_event_publish_strategy strat;
  public:
    template<typename event>
      void operator()(const event &ev,
          const std::vector< event_subscriber<event> * > &subscribers)
      {
        auto fut = std::async(std::launch::async, strat, ev, subscribers);
      }
};

template<typename publish_strategy = synchronous_event_publish_strategy>
class event_bus
{
  private:
    typedef std::vector<void *> subscribers_type;
    typedef subscribers_type::iterator subscribers_iterator;
    std::map<std::type_index, subscribers_type> event_subscriber_map;
    publish_strategy strategy;
  public:
    template<typename event>
      event_bus &subscribe(event_subscriber<event> *subscriber)
      {
        subscribers_type &subscribers = event_subscriber_map[std::type_index(typeid(event))];
        subscribers.push_back(reinterpret_cast<void*>(subscriber));
        return *this;
      }

    template<typename event>
      event_bus &unsubscribe(event_subscriber<event> *subscriber)
      {
        subscribers_type &subscribers = event_subscriber_map[std::type_index(typeid(event))];
        for(auto position = subscribers.begin();
            position != subscribers.end(); position++)
        {
          if(reinterpret_cast<event_subscriber<event> *>(*position) != subscriber)
          {
            subscribers.erase(position);
          }
        }
        return *this;
      }

    template<typename event>
      event_bus &publish(const event &ev)
      {
        subscribers_type &subscribers = event_subscriber_map[std::type_index(typeid(event))];
        std::vector< event_subscriber<event> * > publish_to;
        for(subscribers_iterator subscriber = subscribers.begin();
            subscriber != subscribers.end();
            subscriber++)
        {
          event_subscriber<event> *who = reinterpret_cast< event_subscriber<event> *>(*subscriber);
          publish_to.push_back(who);
        }
        strategy(ev, publish_to);
        return *this;
      }
};

typedef event_bus<synchronous_event_publish_strategy> synchro_event_bus;
typedef event_bus<asynchronous_event_publish_strategy> asynchro_event_bus;
}// namespace event_bus
