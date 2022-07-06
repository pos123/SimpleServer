#pragma once

#include "net_common.h"
#include "thread_safe_queue.h"
#include "connection.h"
#include "connection_interfaces.h"

namespace async_component
{
	class Server
	{
	public:
		Server(uint16_t port, std::shared_ptr<IConnectStatus> connect_status) : asio_acceptor_(asio_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		{
			connect_status_ = connect_status;
		}

		virtual ~Server()
		{
			stop();
		}

		bool start()
		{
			try
			{
				wait_for_client_connection_async(); 
				thread_context_ = std::thread([this]() { asio_context_.run(); });
				connect_status_->on_server_started();
			}
			catch (const std::exception& ex)
			{
				std::cerr << "Server exception: " << ex.what() << std::endl;
				return false;
			}

			return true;
		}

		void stop()
		{
			// issue stop on the asio context
			asio_context_.stop();

			// wait for thread to exit
			if (thread_context_.joinable())
			{
				thread_context_.join();
			}

			connect_status_->on_server_stopped();
		}

		void send_client_message(std::shared_ptr<Connection> client, const message& message)
		{
			if (client && client->is_connected())
			{
				client->send(message);
			}
			else
			{
				connect_status_->on_client_disconnected(client);
				client.reset();
				active_connections_.erase(std::remove(active_connections_.begin(), active_connections_.end(), client), active_connections_.begin());
			}
		}

		void get_incoming_messages(std::function<void(std::shared_ptr<Connection>, message)> callback, size_t max_messages = -1)
		{
			size_t message_count = 0;
			while (message_count < max_messages && !incomming_messages_.empty())
			{
				auto message = incomming_messages_.pop_front();
				callback(message.remote_connection, message);
				++message_count;
			}
		}

		size_t get_incomming_queue_size()
		{
			return incomming_messages_.size();
		}


	private:
		void wait_for_client_connection_async()
		{
			asio_acceptor_.async_accept(
				[this](std::error_code ec, boost::asio::ip::tcp::socket socket)
				{
					if (!ec)
					{
						auto new_connection = std::make_shared<Connection>(asio_context_, std::move(socket), incomming_messages_, connect_status_);

						if (connect_status_->on_client_connected(new_connection))
						{
							active_connections_.push_back(new_connection);
							active_connections_.back()->connect_to_client();
						}
						else
						{
							connect_status_->on_connection_denied(new_connection);
						}

					}
					else
					{
						connect_status_->on_error_code(ec);
					}

					// add another task for the io_context to wait for another connection
					wait_for_client_connection_async();
				});
		}

		std::deque<std::shared_ptr<Connection>> active_connections_;
		ThreadSafeQueue<message> incomming_messages_;
		boost::asio::io_context asio_context_;
		std::thread thread_context_;
		boost::asio::ip::tcp::acceptor asio_acceptor_;
		std::shared_ptr<IConnectStatus> connect_status_;
	};

}