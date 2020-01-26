#include "events.hpp"

#include <stdexcept>

namespace events {

EventEmitter::EventEmitter() {}
EventEmitter::~EventEmitter() {}

unsigned int EventEmitter::AddListener(unsigned int event_id,
                                       std::function<void()> cb, bool once) {
  if (!cb) {
    throw std::invalid_argument(
        "EventEmitter::add_listener: No callbak provided.");
  }

  std::lock_guard<std::mutex> lock(mutex);

  unsigned int listener_id = ++last_listener;
  listeners.insert(std::make_pair(
      event_id, std::make_shared<Listener<>>(listener_id, cb, once)));

  return listener_id;
}

unsigned int EventEmitter::On(unsigned int event_id, std::function<void()> cb) {
  return AddListener(event_id, cb);
}

void EventEmitter::RemoveListener(unsigned int listener_id) {
  std::lock_guard<std::mutex> lock(mutex);
  auto i = std::find_if(
      listeners.begin(), listeners.end(),
      [&](std::pair<const unsigned int, std::shared_ptr<ListenerBase>> p) {
        return p.second->id == listener_id;
      });
  if (i != listeners.end()) {
    listeners.erase(i);
  } else {
    throw std::invalid_argument(
        "EventEmitter::remove_listener: Invalid listener id.");
  }
}

};  // namespace events