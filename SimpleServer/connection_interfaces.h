#pragma once

#include "net_common.h"
#include "message.h"
#include "connection.h"

namespace async_component
{
	class IConnectStatus
	{
	public:
		virtual bool on_client_connected(std::shared_ptr<Connection> client) = 0;
		virtual void on_client_disconnected(std::shared_ptr<Connection> client) = 0;
		virtual void on_server_started() = 0;
		virtual void on_server_stopped() = 0;
		virtual void on_error_code(std::error_code ec) = 0;
		virtual void on_connection_denied(std::shared_ptr<Connection> client_connection) = 0;
	};
}


