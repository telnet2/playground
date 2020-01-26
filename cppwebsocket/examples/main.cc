#include <iostream>
#include <string>
#include <typeindex>
#include <typeinfo>

#include <websocketpp/client.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "util/events.hpp"

using namespace std;
using events::EventEmitter;

typedef websocketpp::client<websocketpp::config::asio_client> client;

struct OpenEvent {};

class ConnectionMetadata {
 public:
  typedef websocketpp::lib::shared_ptr<ConnectionMetadata> ptr;

  ConnectionMetadata(int id, websocketpp::connection_hdl hdl, std::string uri)
      : m_id(id),
        m_hdl(hdl),
        m_status("Connecting"),
        m_uri(uri),
        m_server("N/A") {}

  void OnOpen(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Open";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_event.Emit<>(0);
  }

  void OnFail(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
  }

  void RecordSentMessages(std::string message) {
    m_messages.push_back(">> " + message);
  }

  void OnMessage(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
      m_messages.push_back(msg->get_payload());
    } else {
      m_messages.push_back(websocketpp::utility::to_hex(msg->get_payload()));
    }
  }

  void OnClose(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " ("
      << websocketpp::close::status::get_string(con->get_remote_close_code())
      << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
  }

  EventEmitter *GetEvent() { return &m_event; }
  const int GetId() const { return m_id; }
  const std::string &GetStatus() const { return m_status; }
  const websocketpp::connection_hdl GetHandle() const { return m_hdl; }

  friend std::ostream &operator<<(std::ostream &out,
                                  ConnectionMetadata const &data);

 private:
  int m_id;
  websocketpp::connection_hdl m_hdl;
  std::string m_status;
  std::string m_uri;
  std::string m_server;
  std::string m_error_reason;
  std::vector<std::string> m_messages;
  EventEmitter m_event;
};

std::ostream &operator<<(std::ostream &out, ConnectionMetadata const &data) {
  out << "> URI: " << data.m_uri << "\n"
      << "> Status: " << data.m_status << "\n"
      << "> Remote Server: "
      << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
      << "> Error/close reason: "
      << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);

  out << "> Messages Processed: (" << data.m_messages.size() << ") \n";
  std::vector<std::string>::const_iterator it;
  for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
    out << *it << "\n";
  }
  return out;
}

class WebsocketEndpoints {
 public:
  WebsocketEndpoints() : m_next_id(0) {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);
    m_endpoint.init_asio();
    m_endpoint.start_perpetual();
    m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
  }

  ~WebsocketEndpoints() {
    m_endpoint.stop_perpetual();
    for (ConnectionList::const_iterator it = m_connection_list.begin();
         it != m_connection_list.end(); ++it) {
      if (it->second->GetStatus() != "Open") {
        // Only close open connections
        continue;
      }
      std::cout << "> Closing connection " << it->second->GetId() << std::endl;
      websocketpp::lib::error_code ec;
      m_endpoint.close(it->second->GetHandle(),
                       websocketpp::close::status::going_away, "", ec);
      if (ec) {
        std::cout << "> Error closing connection " << it->second->GetId()
                  << ": " << ec.message() << std::endl;
      }
    }
    m_thread->join();
  }

  /**
   * Returns the id of this websocket.
   */
  int Connect(std::string const &uri) {
    websocketpp::lib::error_code ec;

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);
    if (ec) {
      std::cout << "> Connect initialization error: " << ec.message()
                << std::endl;
      return -1;
    }

    int new_id = m_next_id++;

    ConnectionMetadata::ptr metadata_ptr(
        new ConnectionMetadata(new_id, con->get_handle(), uri));
    m_connection_list[new_id] = metadata_ptr;

    con->set_open_handler(websocketpp::lib::bind(
        &ConnectionMetadata::OnOpen, metadata_ptr, &m_endpoint,
        websocketpp::lib::placeholders::_1));

    con->set_fail_handler(websocketpp::lib::bind(
        &ConnectionMetadata::OnFail, metadata_ptr, &m_endpoint,
        websocketpp::lib::placeholders::_1));

    con->set_message_handler(
        websocketpp::lib::bind(&ConnectionMetadata::OnMessage, metadata_ptr,
                               websocketpp::lib::placeholders::_1,
                               websocketpp::lib::placeholders::_2));

    m_endpoint.connect(con);

    return new_id;
  }

  void Send(int id, std::string message) {
    websocketpp::lib::error_code ec;
    ConnectionList::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
      std::cout << "> No connection found with id " << id << std::endl;
      return;
    }
    m_endpoint.send(metadata_it->second->GetHandle(), message,
                    websocketpp::frame::opcode::text, ec);
    if (ec) {
      std::cout << "> Error sending message: " << ec.message() << std::endl;
      return;
    }
    metadata_it->second->RecordSentMessages(message);
  }

  void Close(int id, websocketpp::close::status::value code) {
    websocketpp::lib::error_code ec;
    ConnectionList::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
      std::cout << "> No connection found with id " << id << std::endl;
      return;
    }
    m_endpoint.close(metadata_it->second->GetHandle(), code, "", ec);
    if (ec) {
      std::cout << "> Error initiating close: " << ec.message() << std::endl;
    }
  }

  ConnectionMetadata::ptr GetMetadata(int id) const {
    ConnectionList::const_iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
      return ConnectionMetadata::ptr();
    } else {
      return metadata_it->second;
    }
  }

  EventEmitter *GetEvent(int id) { return GetMetadata(id)->GetEvent(); }

 private:
  typedef std::map<int, ConnectionMetadata::ptr> ConnectionList;
  client m_endpoint;
  websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
  ConnectionList m_connection_list;
  int m_next_id;
};

int main() {
  WebsocketEndpoints sockets;
  const int id = sockets.Connect("ws://echo.websocket.org");
  auto *event = sockets.GetEvent(id);

  event->On<>(0, std::function<void()>([&]() -> void {
                cout << "Websocket ID:" << id << endl;
                cout << *sockets.GetMetadata(id) << endl;
                sockets.Close(id, 1000);
              }));
}