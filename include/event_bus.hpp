/**
 * This source code is governed by by the boost v1 licensing terms and conditions.
 * Copyright 2016 Nicholas A. Smith (nickrmc83@gmail.com)
 */
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <future>
#include <algorithm>

namespace events
{
  class event_subscriber_base{};

  /**
   * event subscribers should derive from the event_subscriber class
   * directly and not event_subscriber_base.
   */
  template<typename event>
    class event_subscriber : public event_subscriber_base
  {
    public:
      virtual void operator()(const event &ev) noexcept = 0;
  };

  /**
   * Exception class for double subscription errors.
   */
  class already_subscribed_exception : public std::exception
  {
    private:
      std::string error;
      already_subscribed_exception(const std::string &who)
      {
        error = "subscriber (" + who + ") already subscribed";
      }

    public:
      template<typename T>
        static already_subscribed_exception create_exception(const T &ref)
        {
          return already_subscribed_exception(typeid(ref).name());
        }

      const char *what() const noexcept
      {
        return error.c_str();
      }
  };

  /**
   * Synchronous event publishing strategy.
   */
  class synchronous_event_publish_strategy
  {
    public:
      template<typename event>
        void operator()(const event &ev,
            const std::vector< std::shared_ptr< event_subscriber<event> > > &subscribers)
        {
          for(auto subscriber = subscribers.begin();
              subscriber != subscribers.end();
              subscriber++)
          {
            (*subscriber)->operator()(ev);
          }
        }
  };

  /**
   * Asynchronous event publishing strategy.
   */
  class asynchronous_event_publish_strategy
  {
    private:
      synchronous_event_publish_strategy strat;
    public:
      template<typename event>
        void operator()(const event &ev,
            const std::vector< std::shared_ptr< event_subscriber<event> > > &subscribers)
        {
          std::async(std::launch::async, strat, ev, subscribers);
        }
  };

  /**
   * Event bus class.
   */
  template<typename publish_strategy = synchronous_event_publish_strategy>
    class event_bus
    {
      private:
        typedef std::vector<std::shared_ptr<event_subscriber_base>> subscribers_type;
        typedef subscribers_type::iterator subscribers_iterator;
        std::map<std::type_index, subscribers_type> event_subscriber_map;
        publish_strategy strategy;

        template<typename T>
          struct null_deleter
          {
            void operator()(T *obj) const
            {
              // Do nothing;
            }
          };

        // Get a shared ptr that uses a null_deleter.
        template<typename T>
          std::shared_ptr<T> get_shared(T *obj)
          {
            return std::shared_ptr<T>(obj, null_deleter<T>());
          }

      public:

        template<typename event>
          event_bus &subscribe(std::shared_ptr<event_subscriber<event>> &subscriber)
          {
            auto &subscribers = event_subscriber_map[std::type_index(typeid(event))];
            auto where = std::find(subscribers.begin(), subscribers.end(), subscriber);
            if(where != subscribers.end())
            {
              throw already_subscribed_exception::create_exception(subscriber);
            }
            subscribers.push_back(subscriber);
            return *this;
          }

        template<typename event>
          event_bus &subscribe(event_subscriber<event> *subscriber)
          {
            // Subscribe using a smart_pointer that will not delete the pointer.
            auto shared = get_shared(subscriber);
            return subscribe(shared);
          }

        template<typename event>
          event_bus &unsubscribe(std::shared_ptr<event_subscriber<event>> &subscriber)
          {
            auto &subscribers = event_subscriber_map[std::type_index(typeid(event))];
            auto where = std::find(subscribers.begin(), subscribers.end(), subscriber);
            if(where != subscribers.end())
            {
              subscribers.erase(where);
            }
            return *this;
          }

        template<typename event>
          event_bus &unsubscribe(event_subscriber<event> *subscriber)
          {
            auto shared = get_shared(subscriber);
            return unsubscribe(shared);
          }

        template<typename event>
          event_bus &publish(const event &ev)
          {
            auto &subscribers = event_subscriber_map[std::type_index(typeid(event))];
            std::vector< std::shared_ptr< event_subscriber<event> > > publish_to;
            for(subscribers_iterator subscriber = subscribers.begin();
                subscriber != subscribers.end();
                subscriber++)
            {
              auto who =
                std::static_pointer_cast< event_subscriber<event> >(*subscriber);
              publish_to.push_back(who);
            }
            strategy(ev, publish_to);
            return *this;
          }
    };

  /**
   * Synchronous event publishing.
   */
  typedef event_bus<synchronous_event_publish_strategy> synchro_event_bus;
  /**
   * Asynchronous event publishing.
   */
  typedef event_bus<asynchronous_event_publish_strategy> asynchro_event_bus;
}// namespace events
