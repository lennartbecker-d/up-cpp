#include "../../../../include/up-cpp/core/usubscription/handlers/subscribe.h"
#include <uprotocol/v1/ustatus.pb.h>
#include "up-cpp/datamodel/builder/Payload.h"


// Exception Type for ServiceInvocationError
class ServiceInvocationError : public std::runtime_error {
public:
    explicit ServiceInvocationError(const std::string& message) : std::runtime_error(message) {}

    static ServiceInvocationError InvalidArgument(const std::string& message) {
        return ServiceInvocationError("InvalidArgument: " + message);
    }
};
    
    
// From helpers.rs    
// template <typename T>
std::pair<uprotocol::datamodel::builder::Payload, uprotocol::v1::UUri> extract_inputs(
    uint16_t expected_resource_id,
    uint16_t resource_id,
    const uprotocol::v1::UMessage& message) {

    auto payload = message.has_payload() ? std::make_optional(message.payload()) : std::nullopt;
    auto message_attributes = message.has_attributes() ? std::optional<uprotocol::v1::UAttributes>(message.attributes()) : std::nullopt;

    if (resource_id != expected_resource_id) {
        throw ServiceInvocationError::InvalidArgument(
            "Wrong resource ID (expected " + std::to_string(expected_resource_id) + ", got " + std::to_string(resource_id) + ")");
    }

    if (!payload) {
        throw ServiceInvocationError::InvalidArgument("No request payload");
    }

    std::optional<uprotocol::datamodel::builder::Payload> request;
    try {
    // TODO(lennart)
        // request = payload.extract_protobuf()
    } catch (const std::exception& e) {
        throw ServiceInvocationError::InvalidArgument(
            "Expected NotificationsRequest payload, error when unpacking: " + std::string(e.what()));
    }

    const auto& source = message_attributes->source();
    if (!message_attributes) {
        throw ServiceInvocationError::InvalidArgument("No request source uri");
    }

    // TODO(lennart) get corresponding request Payload Constructor, in Rust MessageFull is used, that originates from: request = payload.extract_protobuf()...
    return {uprotocol::datamodel::builder::Payload(request), uprotocol::v1::UUri(source)};

}


namespace uprotocol::core::usubscription::handlers {

utils::Expected<datamodel::builder::Payload, v1::UStatus> subscribe(const v1::UMessage& message) {
    uint16_t expected_resource_id = 0;
    uint16_t received_resource_id = 0;

    auto extracted_data = extract_inputs(expected_resource_id, received_resource_id, message);
    auto subscription_request = extracted_data.first;
    auto source = extracted_data.second; 
    
     // TODO(lennart)
    // if (!subscription_request.topic) {
    //     throw ServiceInvocationError::InvalidArgument("No topic defined in request");
    // }

    // Interact with subscription manager backend
    // let (respond_to, receive_from) = oneshot::channel::<SubscriptionStatus>();
    v3::SubscriptionEvent se = v3::AddSubscription{
        source,
        source  // should be topic, please change as soon it exists
        // Missing respond_to
    };

    try {
        // Send subscription with se from v3::SubscriptionEvent with subscription_sender from struct SubscriptionRequestHandler
    } catch (const std::exception& e) {
        std::cerr << "Error communicating with subscription manager: " << e.what() << std::endl;
        return ServiceInvocationError("Error processing request"); // UStatus zurückgeben
    }
    try {
        // Receive asynchronos response, retrieve subscription status (UStatus?)
    } catch (const std::exception& e) {
        std::cerr << "Error processing request: " << e.what() << std::endl;
        return ServiceInvocationError("Error processing request");
    }

    // Notify update channel
    // -> NotificationEvent StateChange/ Use notification_sender from struct SubscriptionRequestHandler
    
    // Build and return result 
    std::string test_payload_str = "test_payload";
	std::optional<uprotocol::datamodel::builder::Payload> response_payload;
    try {
        uprotocol::datamodel::builder::Payload response_payload(
            test_payload_str, 
            uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
        } catch (const std::exception& e) {
        std::cerr << "Error building response payload: " << e.what() << std::endl;
        return ServiceInvocationError("Error building response payload");
    }
    
    return response_payload;
};

} // namespace uprotocol::core::usubscription::handlers
