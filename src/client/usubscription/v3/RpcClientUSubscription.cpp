// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#include <up-cpp/client/usubscription/v3/RpcClientUSubscription.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <utility>

namespace uprotocol::core::usubscription::v3 {

void someCallBack(const uprotocol::v1::UMessage& message) {
	// Print the message
	std::cout << message.DebugString() << std::endl;
}

RpcClientUSubscription::RpcClientUSubscription(std::shared_ptr<uprotocol::transport::UTransport> transport,
                   v1::UUri subscription_topic,
                   RpcClientUSubscriptionOptions rpc_client_usubscription_options)
    : transport_(std::move(transport)),
      subscription_topic_(std::move(subscription_topic)),
      rpc_client_usubscription_options_(std::move(rpc_client_usubscription_options)),
      rpc_client_(nullptr) {
	// Initialize uSubscriptionUUriBuilder_
	uSubscriptionUUriBuilder_ = USubscriptionUUriBuilder();
}

// [[nodiscard]] RpcClientUSubscription::RpcClientUSubscriptionOrStatus RpcClientUSubscription::create(
//     std::shared_ptr<transport::UTransport> transport,
//     const v1::UUri& subscription_topic, ListenCallback&& callback,
//     v1::UPriority priority, std::chrono::milliseconds subscription_request_ttl,
//     RpcClientUSubscriptionOptions rpc_client_usubscription_options) {
// 	auto rpc_client_usubscription = std::make_unique<RpcClientUSubscription>(
// 	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
// 	    std::forward<const v1::UUri>(subscription_topic),
// 	    std::forward<RpcClientUSubscriptionOptions>(rpc_client_usubscription_options));

// 	// Attempt to connect create notification sink for updates.
// 	auto status = rpc_client_usubscription->createNotificationSink();
// 	if (status.code() == v1::UCode::OK) {
// 		status = rpc_client_usubscription->subscribe(priority, subscription_request_ttl,
// 		                             std::move(callback));
// 		if (status.code() == v1::UCode::OK) {
// 			return RpcClientUSubscriptionOrStatus(std::move(rpc_client_usubscription));
// 		}
// 		return RpcClientUSubscriptionOrStatus(utils::Unexpected<v1::UStatus>(status));
// 	}
// 	// If connection fails, return the error status.
// 	return RpcClientUSubscriptionOrStatus(utils::Unexpected<v1::UStatus>(status));
// }

v1::UStatus RpcClientUSubscription::createNotificationSink() {
	auto notification_sink_callback = [this](const v1::UMessage& update) {
		if (update.has_payload()) {
			Update data;
			if (data.ParseFromString(update.payload())) {
				if (data.topic().SerializeAsString() ==
				    subscription_topic_.SerializeAsString()) {
					subscription_update_ = std::move(data);
				}
			}
		}
	};

	auto notification_topic = uSubscriptionUUriBuilder_.getNotificationUri();

	auto result = communication::NotificationSink::create(
	    transport_, std::move(notification_sink_callback), notification_topic);

	if (result.has_value()) {
		noficationSinkHandle_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	}
	return result.error();
}

SubscriptionRequest RpcClientUSubscription::buildSubscriptionRequest() {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	    rpc_client_usubscription_options_.when_expire, rpc_client_usubscription_options_.subscription_details,
	    rpc_client_usubscription_options_.sample_period_ms);

	auto subscription_request = utils::ProtoConverter::BuildSubscriptionRequest(
	    subscription_topic_, attributes);
	return subscription_request;
}

void RpcClientUSubscription::Subscribe(
	google::protobuf::RpcController* controller,
	const ::uprotocol::core::usubscription::v3::SubscriptionRequest* request,
	::uprotocol::core::usubscription::v3::SubscriptionResponse* response,
	::google::protobuf::Closure* done) {
	
	constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
	auto subscription_request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4; // TODO(lennart) priority

	auto options = uprotocol::core::usubscription::v3::RpcClientUSubscriptionOptions();


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

	// SubscriptionRequest const subscription_request = buildSubscriptionRequest();
	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct, has been subscription_request before

	rpc_handle_ =
	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

	// response->CopyFrom(rpc_handle_.value);	

	auto subscription_callback = someCallBack;
	// Create a L2 subscription
	auto result = communication::Subscriber::subscribe(
	    transport_, subscription_topic_, std::move(subscription_callback));

	if (result.has_value()) {
		subscriber_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		// return status;
		done->Run();
	}
	controller->SetFailed("result.error()");
	done->Run();
}

UnsubscribeRequest RpcClientUSubscription::buildUnsubscriptionRequest() {
	auto unsubscribe_request =
	    utils::ProtoConverter::BuildUnSubscribeRequest(subscription_topic_);
	return unsubscribe_request;
}

void RpcClientUSubscription::Unsubscribe(
	google::protobuf::RpcController* controller,
	const ::uprotocol::core::usubscription::v3::UnsubscribeRequest* request,
	::uprotocol::core::usubscription::v3::UnsubscribeResponse* response,
	::google::protobuf::Closure* done) {
	
	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
	constexpr uint16_t RESOURCE_ID_UNSUBSCRIBE = 0x0002;
	auto request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4; // TODO(lennart) priority
	
	rpc_client_ = std::make_unique<communication::RpcClient>(
	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_UNSUBSCRIBE),
	    priority, request_ttl);

	auto on_response = [this, response](const auto& maybe_response) {
		if (maybe_response.has_value() &&
		    maybe_response.value().has_payload()) {
			if (response->ParseFromString(maybe_response.value().payload())) {
				// if (response->topic().SerializeAsString() == // TODO(lennart) see if this check is somehow possible
				//     subscription_topic_.SerializeAsString()) { 
					unsubscribe_response_ = *response;
				// }
			}
		}
	};

	// UnsubscribeRequest const unsubscribe_request = buildUnsubscriptionRequest();
	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct, has been subscription_request before

	rpc_handle_ =
	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

	subscriber_.reset();

	done->Run();	
}

}  // namespace uprotocol::core::usubscription::v3
