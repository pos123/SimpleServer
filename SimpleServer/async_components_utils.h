#pragma once

#include "net_common.h"

namespace async_component
{
	class utils
	{
	public:
		static void initialise_data_memory(std::vector<uint8_t>& data, size_t size)
		{
			data.clear();
			data.resize(size);
			std::memset(data.data(), ' ', size);
		}

		static void write_data_value(std::vector<uint8_t>& destination, std::vector<uint8_t>& input_data)
		{
			std::memcpy(destination.data(), input_data.data(), input_data.size());
		}

		static void write_data_value(std::vector<uint8_t>& destination, const char* value, size_t size)
		{
			std::memcpy(destination.data(), value, size);
		}

		static void write_data_value(std::vector<uint8_t>& destination, const std::string& input)
		{
			write_data_value(destination, input.c_str(), input.size());
		}

		static size_t get_payload_size_from_header(std::vector<uint8_t>& data, size_t size)
		{
			std::string payload_size(reinterpret_cast<const char*>(data.data()), size);
			return std::stoi(payload_size);
		}

		static std::string convert_to_string(const std::vector<uint8_t>& data)
		{
			return std::string(reinterpret_cast<const char*>(data.data()), data.size());
		}
	};
}