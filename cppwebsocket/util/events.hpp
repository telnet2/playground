#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>

namespace events {

class EventEmitter {
 public:
  EventEmitter();
  ~EventEmitter();

  template <typename... Args>
  unsigned int AddListener(unsigned int event_id,
                           std::function<void(Args...)> cb, bool once = false);
  unsigned int AddListener(unsigned int event_id, std::function<void()> cb,
                           bool once = false);

  void RemoveListener(unsigned int listener_id);

  template <typename... Args>
  unsigned int On(unsigned int event_id, std::function<void(Args...)> cb);
  unsigned int On(unsigned int event_id, std::function<void()> cb);

  template <typename... Args>
  void Emit(unsigned int event_id, Args... args);

 private:
  struct ListenerBase {
    ListenerBase() {}
    ListenerBase(unsigned int i) : id(i) {}
    virtual ~ListenerBase() {}
    unsigned int id;
  };

  template <typename... Args>
  struct Listener : public ListenerBase {
    Listener() : once(false) {}
    Listener(unsigned int i, std::function<void(Args...)> c, bool once)
        : ListenerBase(i), cb(c), once(once) {}
    std::function<void(Args...)> cb;
    bool once;
  };

  std::mutex mutex;
  unsigned int last_listener;
  std::multimap<unsigned int, std::shared_ptr<ListenerBase>> listeners;

  EventEmitter(const EventEmitter&) = delete;
  const EventEmitter& operator=(const EventEmitter&) = delete;
};

template <typename... Args>
unsigned int EventEmitter::AddListener(unsigned int event_id,
                                       std::function<void(Args...)> cb,
                                       bool once) {
  if (!cb) {
    throw std::invalid_argument(
        "EventEmitter::AddListener: No callbak provided.");
  }

  std::lock_guard<std::mutex> lock(mutex);

  unsigned int listener_id = ++last_listener;
  listeners.insert(std::make_pair(
      event_id, std::make_shared<Listener<Args...>>(listener_id, cb, once)));

  return listener_id;
}

template <typename... Args>
unsigned int EventEmitter::On(unsigned int event_id,
                              std::function<void(Args...)> cb) {
  return AddListener(event_id, cb);
}

template <typename... Args>
void EventEmitter::Emit(unsigned int event_id, Args... args) {
  std::list<std::shared_ptr<Listener<Args...>>> handlers;
  {
    std::lock_guard<std::mutex> lock(mutex);

    auto range = listeners.equal_range(event_id);
    handlers.resize(std::distance(range.first, range.second));

    std::transform(
        range.first, range.second, handlers.begin(),
        [](std::pair<const unsigned int, std::shared_ptr<ListenerBase>> p) {
          auto l = std::dynamic_pointer_cast<Listener<Args...>>(p.second);
          if (l) {
            return l;
          } else {
            throw std::logic_error(
                "EventEmitter::emit: Invalid event signature.");
          }
        });
  }

  std::vector<unsigned int> to_remove;
  for (auto& h : handlers) {
    if (h->once) {
      to_remove.push_back(h->id);
    }
    h->cb(args...);
  }
  for (auto id : to_remove) {
    RemoveListener(id);
  }
}

};  // namespace events