#include "../../../../include/up-cpp/core/usubscription/handlers/subscribe.h"


//                          EXCEPTION HANDLING

// Exception Type for ServiceInvocationError
class ServiceInvocationError : public std::runtime_error {
public:
    explicit ServiceInvocationError(const std::string& message) : std::runtime_error(message) {}

    static ServiceInvocationError InvalidArgument(const std::string& message) {
        return ServiceInvocationError("InvalidArgument: " + message);
    }
};
    
    
    
// Extracting inputs
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
    auto request = payload;
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

    return {uprotocol::datamodel::builder::Payload(request), uprotocol::v1::UUri(source)};

}


namespace uprotocol::core::usubscription::handlers {

std::optional<datamodel::builder::Payload> subscribe(const v1::UMessage& message) {
    uint16_t expected_resource_id = 0;
    uint16_t received_resource_id = 0;

    auto extracted_data = extract_inputs(expected_resource_id, received_resource_id, message); 
    
    std::string test_payload_str = "test_payload";
	uprotocol::datamodel::builder::Payload response_payload(test_payload_str,
	                                                    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

    return std::make_optional(response_payload);
};

} // namespace uprotocol::core::usubscription::handlers
