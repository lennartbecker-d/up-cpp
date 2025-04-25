//
// Created by max on 23.04.25.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <string>

namespace uprotocol::core::usubscription::v3 {
	struct USubscriptionConfiguration {

		USubscriptionConfiguration create(std::string, size_t, size_t);

	private:
		std::string authority_name;
		size_t subscription_command_buffer_size;
		size_t notification_command_buffer_size;
	};


} // namespace uprotocol::core::usubscription::v3
#endif //CONFIGURATION_H
