#include "stdafx.h"
#include "async_components.h"


class ConnectionStatus : public async_component::IConnectStatus
{
public:    
    bool on_client_connected(std::shared_ptr<async_component::Connection> client) override
    {
        std::cout << "client connected" << std::endl;
        return true;
    }
    
    void on_client_disconnected(std::shared_ptr<async_component::Connection> client) override
    {
        std::cout << "client disconnected" << std::endl;
    }
    
    void on_server_started() override
    {
        std::cout << "on server started" << std::endl;
    }
    
    void on_server_stopped() override
    {
        std::cout << "on server stopped" << std::endl;
    }

    void on_error_code(std::error_code ec) override
    {
        std::cout << "on error code: " << ec.message() << std::endl;
    }
    
    void on_connection_denied(std::shared_ptr<async_component::Connection> client_connection) override
    {
        std::cout << "connection denied" << std::endl;
    }
};


int main()
{
    auto connection_status = std::make_shared<ConnectionStatus>();
    async_component::Server server(1236, connection_status);
    server.start();

    while (true)
    {
        server.get_incoming_messages([&server](const std::shared_ptr<async_component::Connection> connection, const async_component::message& message)
        {
            auto payload = async_component::utils::convert_to_string(message.payload);
            std::cout << "got payload: " << payload << std::endl;
            std::cout << "queue size: " << server.get_incomming_queue_size() << std::endl;
            auto message_send = async_component::message::create_message("hello yourself");
            server.send_client_message(connection, message_send);
        });
    }

    system("pause > nul");
    return 0;
}


/*
    auto message = std::make_unique<async_component::message>();

    message->initialise_header();
    message->initialise_payload(5);

    std::string payload = "hello";
    message->write_payload(payload.data(), payload.size());
    auto message2 = std::move(message);
    message.reset();

   *   How to get end point b from dns name

       ip::tcp::resolver resolver(my_io_service);
       ip::tcp::resolver::query query("www.boost.org", "http");
       ip::tcp::resolver::iterator iter = resolver.resolve(query);
       ip::tcp::resolver::iterator end; // End marker.
       while (iter != end)
       {
           ip::tcp::endpoint endpoint = *iter++;
           std::cout << endpoint << std::endl;
       }
   */


   //asio::error_code ec;

   //// creates a context which creates a platform specific interface to do stuff in
   //asio::io_context context;

   //// address endpoint
   //asio::ip::tcp::endpoint endpoint(asio::ip::make_address("2606:2800:220:1:248:1893:25c8:1946", ec), 80);

   //asio::ip::tcp::socket socket(context);

   //socket.connect(endpoint, ec);

   //if (ec)
   //{
   //    std::cout << "not connected" << ec.message() << std::endl;
   //}

   //if (socket.is_open())
   //{
   //    std::string request = "GET /index.html HTTP/1.1\r\n"
   //        "HOST: example.com\r\n"
   //        "Connection: close\r\n\r\n";
   //    socket.write_some(asio::buffer(request.data(), request.size()), ec);

   //    using namespace std::chrono_literals;
   //    std::this_thread::sleep_for(200ms);

   //    auto bytes = socket.available();
   //    std::cout << "bytes available: " << bytes << std::endl;

   //    if (bytes > 0)
   //    {
   //        std::vector<char> buffer(bytes);
   //        socket.read_some(asio::buffer(buffer.data(), buffer.size()), ec);

   //        for (auto c : buffer)
   //        {
   //            std::cout << c;
   //        }
   //    }
   //}