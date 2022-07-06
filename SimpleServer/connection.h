#pragma once

#include "net_common.h"
#include "thread_safe_queue.h"
#include "message.h"
#include "async_components_utils.h"
#include "connection_interfaces.h"


namespace async_component
{
	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		Connection(boost::asio::io_context& asio_context, 
			boost::asio::ip::tcp::socket&& socket, 
			ThreadSafeQueue<message>& incoming_messages_queue,
			std::shared_ptr<IConnectStatus> connect_status)
			:	asio_context_(asio_context),
				socket_(std::move(socket)),
				incoming_messages_queue_(incoming_messages_queue)
		{
			connect_status_ = connect_status;
		}

		virtual ~Connection()
		{
		}
				
		bool disconnect() 
		{
			if (is_connected())
			{
				boost::asio::post(asio_context_, [this] {socket_.close(); });
			}
		}

		bool is_connected() const
		{
			return socket_.is_open();
		}
		
		bool send(const message& message)
		{
			boost::asio::post(asio_context_, [this, message]() 
			{
				const auto writing_message = !outgoing_messages_queue_.empty();
				outgoing_messages_queue_.push_back(message);
				if (!writing_message)
				{
					write_header_async();
				}
			});

			return true;
		}
		
		void connect_to_client()
		{
			read_header_async();
		}

	private:
		void read_header_async()
		{
			wip_buffer_message_.initialise_header();
			
			boost::asio::async_read(socket_, boost::asio::buffer(wip_buffer_message_.header.size_data.data(), message_header_size),
			[this](std::error_code ec, size_t length)
			{
				if (!ec)
				{
					read_body_async();
				}
				else
				{
					connect_status_->on_error_code(ec);
					// error occurred so close the socket
					socket_.close();
				}
			});
		}

		void read_body_async()
		{
			const auto payload_size = utils::get_payload_size_from_header(wip_buffer_message_.header.size_data, message_header_size);
			wip_buffer_message_.initialise_payload(payload_size);

			boost::asio::async_read(socket_, boost::asio::buffer(wip_buffer_message_.payload.data(), payload_size),
			[this](std::error_code ec, size_t length)
			{
				if (!ec)
				{
					wip_buffer_message_.remote_connection = this->shared_from_this();
					incoming_messages_queue_.push_back(std::move(wip_buffer_message_));
					read_header_async();
				}
				else
				{
					connect_status_->on_error_code(ec);
					// error occurred so close the socket
					socket_.close();
				}
			});

		}

		void write_header_async()
		{
			boost::asio::async_write(socket_, boost::asio::buffer(outgoing_messages_queue_.front().header.size_data.data(), outgoing_messages_queue_.front().header.size_data.size()),
			[this](std::error_code ec, size_t length) 
			{
				if (!ec)
				{
					write_body_async();
				}
				else
				{
					// error - probably socket closed.
					connect_status_->on_error_code(ec);
					socket_.close();
				}
			});

		}

		void write_body_async()
		{
			boost::asio::async_write(socket_, boost::asio::buffer(outgoing_messages_queue_.front().payload.data(), outgoing_messages_queue_.front().payload.size()),
			[this](std::error_code ec, size_t length)
			{
				if (!ec)
				{
					// remove the message from queue as it has now been written to remote
					outgoing_messages_queue_.pop_front();

					// write out the next if there are more messages
					if (!outgoing_messages_queue_.empty())
					{
						write_header_async();
					}

				}
				else
				{
					// error - probably socket closed.
					connect_status_->on_error_code(ec);
					socket_.close();
				}
			});
		}
	
	protected:
		// connection to the remote client socket
		boost::asio::ip::tcp::socket socket_;
		
		// this is the shared asio context
		boost::asio::io_context& asio_context_;

		// outgoing messages queue to be sent to remote side of this connection
		ThreadSafeQueue<message> outgoing_messages_queue_;

		// incoming messages sent to this connection - shared by all connection objects
		// as there is only one for the server
		ThreadSafeQueue<message>& incoming_messages_queue_;

		// work in progress buffer
		message wip_buffer_message_;

		// used to update status
		std::shared_ptr<IConnectStatus> connect_status_;
	};
}
