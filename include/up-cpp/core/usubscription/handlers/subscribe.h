#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H
#include <optional>
#include "up-cpp/communication/NotificationSink.h"
#include "up-cpp/datamodel/builder/Payload.h"
#include "uprotocol/v1/umessage.pb.h"
#include "../subscription_manager.h"

// std::function<std::optional<datamodel::builder::Payload>(
// 	const v1::UMessage&)>;
namespace uprotocol::core::usubscription::handlers{

utils::Expected<datamodel::builder::Payload, v1::UStatus> subscribe(const v1::UMessage& message);

} // namespace uprotocol::core::usubscription::handlers

#endif //SUBSCRIBE_H
