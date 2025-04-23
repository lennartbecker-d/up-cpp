//
// Created by max on 23.04.25.
//

#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H
#include <optional>

#include "up-cpp/communication/NotificationSink.h"
#include "up-cpp/datamodel/builder/Payload.h"
#include "uprotocol/v1/umessage.pb.h"

// std::function<std::optional<datamodel::builder::Payload>(
// 	const v1::UMessage&)>;
namespace uprotocol::core::usubscription::handlers{

std::optional<datamodel::builder::Payload> subscribe(const v1::UMessage&);

} // namespace uprotocol::core::usubscription::handlers
#endif //SUBSCRIBE_H
