//
// Created by max on 23.04.25.
//

#ifndef USUBSCRIPTION_H
#define USUBSCRIPTION_H

#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include "up-cpp/communication/RpcServer.h"

namespace uprotocol::core::usubscription::v3 {

	struct USubscriptionStopper;

	struct USubscriptionConfig;

	struct USubscriptionService : communication::RpcServer{

		explicit USubscriptionService(std::shared_ptr<transport::UTransport> transport, USubscriptionConfig config);

		//TODO(max) make async
		utils::Expected<USubscriptionStopper,v1::UStatus> run();
	};


} // namespace uprotocol::core::usubscription::v3

#endif //USUBSCRIPTION_H
