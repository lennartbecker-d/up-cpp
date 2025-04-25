#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include "configuration.h"

namespace uprotocol::core::usubscription::v3 {
    
// Define events
struct AddSubscription {
    v1::UUri subscriber;
    v1::UUri topic;
    // Sender<int> respond_to;
};

struct RemoveSubscription {
    v1::UUri subscriber;
    v1::UUri topic;
    // Sender<int> respond_to;
};

using SubscriptionEvent = std::variant<AddSubscription, RemoveSubscription>;

    void handle_message(const USubscriptionConfiguration& configuration, const transport::UTransport& transport, const SubscriptionEvent& command_receiver);

} // namespace uprotocol::core::usubscription::v3

#endif //SUBSCRIPTION_MANAGER_H