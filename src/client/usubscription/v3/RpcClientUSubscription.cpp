#include "up-cpp/client/usubscription/v3/RpcClientUSubscription.h"
#include <sys/types.h>
#include <uprotocol/v1/uri.pb.h>

namespace uprotocol::core::usubscription::v3 {
	using Payload = datamodel::builder::Payload;

void RpcClientUSubscription::Subscribe(
	google::protobuf::RpcController* controller,
	const ::uprotocol::core::usubscription::v3::SubscriptionRequest* request,
	::uprotocol::core::usubscription::v3::SubscriptionResponse* response,
	::google::protobuf::Closure* done) {
	
	constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
	// TODO(lennart): needs to be set
	v1::UPriority priority;
	std::chrono::milliseconds subscription_request_ttl;

	if ((request == nullptr) || (response == nullptr) || (done == nullptr)) {
		controller->SetFailed("Invalid input parameters");
		done->Run();
		return;
	}

	rpc_client_ = std::make_unique<communication::RpcClient>(
	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_SUBSCRIBE),
	    priority, subscription_request_ttl);
	
	auto on_response = [this, response](const auto& maybe_response) {
		if (maybe_response.has_value() &&
			maybe_response.value().has_payload()) {
			if (response->ParseFromString(maybe_response.value().payload())) {
				if (response->topic().SerializeAsString() ==
					subscription_topic_.SerializeAsString()) {
					subscription_response_ = *response;
				}
			}
		}
	};

	try {
		auto payload = datamodel::builder::Payload(request);

		rpc_handle_ = rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

		if (static_cast<int>(rpc_handle_.isConnected()) == v1::OK) {		// TODO(lennart): check if this is correct
			response->CopyFrom(rpc_handle_.value);  // Copy data to the response
		} else {
			controller->SetFailed("RPC call failed");
		}
		
	} catch (const std::exception& e) {
		controller->SetFailed(std::string("Exception during Subscribe: ") + e.what());
	}

	done->Run();
}

// Backup
	// SubscriptionResponse RpcClientUSubscription::subscribe(
    // const SubscriptionRequest& subscription_request) {
	// 	Payload test_test(subscription_request);
	// 	auto invoke_handle = client_->invokeMethod(test_test, //TODO(max));
	//     return SubscriptionResponse();
    // }

} // namespace uprotocol::core::usubscription::v3
