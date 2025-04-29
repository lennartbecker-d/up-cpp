//
// Created by max on 28.04.25.
//

#include "up-cpp/client/usubscription/v3/RpcClientUSubscription.h"

namespace uprotocol::core::usubscription::v3 {
	using Payload = datamodel::builder::Payload;

	SubscriptionResponse RpcClientUSubscription::subscribe(
    const SubscriptionRequest& subscription_request) {
		Payload test_test(subscription_request);
		auto invoke_handle = client_->invokeMethod(test_test, //TODO(max));
	    return SubscriptionResponse();
    }

    } // namespace uprotocol::core::usubscription::v3

