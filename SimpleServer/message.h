#pragma once

#include "net_common.h"
#include "async_components_utils.h"

namespace async_component
{
	class Connection;

	struct message_header
	{
		std::vector<uint8_t> size_data;
	};

	// maximum size of 4 byte number
	const size_t message_header_size = 10;

	class message
	{

	public:
		// header
		message_header header;

		// payload
		std::vector<uint8_t> payload;

		// the remote connection from the client
		std::shared_ptr<Connection> remote_connection;
	
		void initialise_header()
		{
			utils::initialise_data_memory(header.size_data, message_header_size);
		}

		void initialise_payload(size_t payload_size)
		{
			utils::initialise_data_memory(payload, payload_size);
		}

		static message create_message(const std::string& payload)
		{
			message msg;
			msg.initialise_header();
			msg.initialise_payload(payload.size());

			utils::write_data_value(msg.header.size_data, std::to_string(payload.size()).c_str(), (size_t)log10(payload.size()) + 1);
			utils::write_data_value(msg.payload, payload.c_str(), payload.size());

			return msg;
		}

		static message create_message(std::vector<uint8_t>& data)
		{
			message msg;
			msg.initialise_header();
			msg.initialise_payload(data.size());

			utils::write_data_value(msg.header.size_data, std::to_string(data.size()).c_str(), (size_t)log10(data.size()) + 1);
			utils::write_data_value(msg.payload, data);

			return msg;
		}

	};
}