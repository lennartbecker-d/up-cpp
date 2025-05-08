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
#include <up-cpp/client/usubscription/v3/USubscription.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <utility>
#include "up-cpp/communication/RpcClient.h"

constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
// TODO(lennart) see default_call_options() for the request in Rust
constexpr auto SUBSCRIPTION_REQUEST_TTL = std::chrono::milliseconds(0x0800); // TODO(lennart) change time
auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;	// MUST be >= 4

namespace uprotocol::core::usubscription::v3 {

RpcClientUSubscription::RpcClientUSubscription(std::shared_ptr<uprotocol::transport::UTransport> transport,
                   RpcClientUSubscriptionOptions rpc_client_usubscription_options)
    : transport_(std::move(transport)),
      rpc_client_usubscription_options_(std::move(rpc_client_usubscription_options)) {
	// Initialize uSubscriptionUUriBuilder_
	uSubscriptionUUriBuilder_ = USubscriptionUUriBuilder();
}

// [[nodiscard]] RpcClientUSubscription::RpcClientUSubscriptionOrStatus RpcClientUSubscription::create(
//     std::shared_ptr<transport::UTransport> transport,
//     const v1::UUri& subscription_topic, ListenCallback&& callback,
//     RpcClientUSubscriptionOptions rpc_client_usubscription_options) {
// 	auto rpc_client_usubscription = std::make_unique<RpcClientUSubscription>(
// 	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
// 	    std::forward<RpcClientUSubscriptionOptions>(rpc_client_usubscription_options));
	
// 	google::protobuf::RpcController *controller = nullptr;
// 	::uprotocol::core::usubscription::v3::SubscriptionRequest const *subscription_request = nullptr;
// 	SubscriptionResponse *subscription_response = nullptr;

// 	// Attempt to connect create notification sink for updates.
// 	auto status = rpc_client_usubscription->createNotificationSink();
// 	if (status.code() == v1::UCode::OK) {
// 		rpc_client_usubscription->Subscribe(controller, subscription_request,
// 			subscription_response, nullptr);
// 		if (controller == nullptr) {
// 			return RpcClientUSubscriptionOrStatus(std::move(rpc_client_usubscription));
// 		}
// 		return RpcClientUSubscriptionOrStatus(utils::Unexpected<v1::UStatus>(status));
// 	}
// 	// If connection fails, return the error status.
// 	return RpcClientUSubscriptionOrStatus(utils::Unexpected<v1::UStatus>(status));
// }

// v1::UStatus RpcClientUSubscription::createNotificationSink() {
// 	auto notification_sink_callback = [this](const v1::UMessage& update) {
// 		if (update.has_payload()) {
// 			Update data;
// 			if (data.ParseFromString(update.payload())) {
// 				if (data.topic().SerializeAsString() ==
// 				    subscription_topic_.SerializeAsString()) {
// 					subscription_update_ = std::move(data);
// 				}
// 			}
// 		}
// 	};

// 	auto notification_topic = uSubscriptionUUriBuilder_.getNotificationUri();

// 	auto result = communication::NotificationSink::create(
// 	    transport_, std::move(notification_sink_callback), notification_topic);

// 	if (result.has_value()) {
// 		noficationSinkHandle_ = std::move(result).value();
// 		v1::UStatus status;
// 		status.set_code(v1::UCode::OK);
// 		return status;
// 	}
// 	return result.error();
// }

SubscriptionRequest RpcClientUSubscription::buildSubscriptionRequest() {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	    rpc_client_usubscription_options_.when_expire, rpc_client_usubscription_options_.subscription_details,
	    rpc_client_usubscription_options_.sample_period_ms);

	auto subscription_request = utils::ProtoConverter::BuildSubscriptionRequest(
	    subscription_topic_, attributes);
	return subscription_request;
}

RpcClientUSubscription::ResponseOrStatus<SubscriptionResponse> RpcClientUSubscription::subscribe(const SubscriptionRequest& subscription_request) {

	communication::RpcClient rpc_client(
	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_SUBSCRIBE),
	    priority, SUBSCRIPTION_REQUEST_TTL);

	datamodel::builder::Payload payload(subscription_request);

	auto invoke_future =
	    rpc_client.invokeMethod(std::move(payload));

	auto message_or_status = invoke_future.get();
	
	if (!message_or_status.has_value()) {
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(message_or_status.error()));
	}

	SubscriptionResponse subscription_response;
	subscription_response.ParseFromString(message_or_status.value().payload());

	if (subscription_response.topic().SerializeAsString() ==
		subscription_topic_.SerializeAsString()) {
		return ResponseOrStatus<SubscriptionResponse>(subscription_response);
	}

	return ResponseOrStatus<SubscriptionResponse>(
		utils::Unexpected<v1::UStatus>(message_or_status.error()));
		
}




// UnsubscribeRequest RpcClientUSubscription::buildUnsubscriptionRequest() {
// 	auto unsubscribe_request =
// 	    utils::ProtoConverter::BuildUnSubscribeRequest(subscription_topic_);
// 	return unsubscribe_request;
// }

// void RpcClientUSubscription::Unsubscribe(
// 	google::protobuf::RpcController* controller,
// 	const ::uprotocol::core::usubscription::v3::UnsubscribeRequest* request,
// 	::uprotocol::core::usubscription::v3::UnsubscribeResponse* response,
// 	::google::protobuf::Closure* done) {
	
// 	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
// 	constexpr uint16_t RESOURCE_ID_UNSUBSCRIBE = 0x0002;
// 	auto request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
// 	auto priority = uprotocol::v1::UPriority::UPRIORITY_UNSPECIFIED;
	
// 	rpc_client_ = std::make_unique<communication::RpcClient>(
// 	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_UNSUBSCRIBE),
// 	    priority, request_ttl);

// 	auto on_response = [this, response](const auto& maybe_response) {
// 		if (maybe_response.has_value() &&
// 		    maybe_response.value().has_payload()) {
// 			if (response->ParseFromString(maybe_response.value().payload())) {
// 				if (response->SerializeAsString() == // TODO(lennart) topic specific? See subscribe
// 				    subscription_topic_.SerializeAsString()) { 
// 					unsubscribe_response_ = *response;
// 				}
// 			}
// 		}
// 	};

// 	// UnsubscribeRequest const unsubscribe_request = buildUnsubscriptionRequest();
// 	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct
// 	rpc_handle_ =
// 	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

// 	// TODO(lennart) any handle for the response?

// 	subscriber_.reset();

// 	done->Run();	
// }

// void RpcClientUSubscription::FetchSubscriptions(
// 	google::protobuf::RpcController* controller,
// 	const ::uprotocol::core::usubscription::v3::FetchSubscriptionsRequest* request,
// 	::uprotocol::core::usubscription::v3::FetchSubscriptionsResponse* response,
// 	::google::protobuf::Closure* done) {
	
// 	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
// 	constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIPTIONS = 0x0003;
// 	auto request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
// 	auto priority = uprotocol::v1::UPriority::UPRIORITY_UNSPECIFIED;
	
// 	rpc_client_ = std::make_unique<communication::RpcClient>(
// 	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_FETCH_SUBSCRIPTIONS),
// 	    priority, request_ttl);

// 	auto on_response = [this, response](const auto& maybe_response) {
// 		if (maybe_response.has_value() &&
// 		    maybe_response.value().has_payload()) {
// 			if (response->ParseFromString(maybe_response.value().payload())) {
// 				if (response->SerializeAsString() == // TODO(lennart) topic specific? See subscribe
// 				    subscription_topic_.SerializeAsString()) { 
// 					fetch_subscription_response_ = *response;
// 				}
// 			}
// 		}
// 	};

// 	// FetchSubscriptionsRequest const fetch_subscriptions_request = buildFetchSubscriptionsRequest();
// 	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct

// 	rpc_handle_ =
// 	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

// 	// TODO(lennart) any handle for the response?

// 	done->Run();	
// }

// void RpcClientUSubscription::RegisterForNotifications(
// 	google::protobuf::RpcController* controller,
// 	const ::uprotocol::core::usubscription::v3::NotificationsRequest* request,
// 	::uprotocol::core::usubscription::v3::NotificationsResponse* response,
// 	::google::protobuf::Closure* done) {
	
// 	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
// 	constexpr uint16_t RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS = 0x0006;
// 	auto request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
// 	auto priority = uprotocol::v1::UPriority::UPRIORITY_UNSPECIFIED;
	
// 	rpc_client_ = std::make_unique<communication::RpcClient>(
// 	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS),
// 	    priority, request_ttl);

// 	auto on_response = [this, response](const auto& maybe_response) {
// 		if (maybe_response.has_value() &&
// 		    maybe_response.value().has_payload()) {
// 			if (response->ParseFromString(maybe_response.value().payload())) {
// 				if (response->SerializeAsString() == // TODO(lennart) topic specific? See subscribe
// 				    subscription_topic_.SerializeAsString()) { 
// 						notification_response_ = *response;
// 				}
// 			}
// 		}
// 	};

// 	// NotificationsRequest const register_notifications_request = buildRegisterNotificationsRequest();
// 	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct

// 	rpc_handle_ =
// 	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

// 	// TODO(lennart) any handle for the response?

// 	done->Run();	
// }

// void RpcClientUSubscription::UnregisterForNotifications(
// 	google::protobuf::RpcController* controller,
// 	const ::uprotocol::core::usubscription::v3::NotificationsRequest* request,
// 	::uprotocol::core::usubscription::v3::NotificationsResponse* response,
// 	::google::protobuf::Closure* done) {
	
// 	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
// 	constexpr uint16_t RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS = 0x0007;
// 	auto request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
// 	auto priority = uprotocol::v1::UPriority::UPRIORITY_UNSPECIFIED;
	
// 	rpc_client_ = std::make_unique<communication::RpcClient>(
// 	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS),
// 	    priority, request_ttl);

// 	auto on_response = [this, response](const auto& maybe_response) {
// 		if (maybe_response.has_value() &&
// 		    maybe_response.value().has_payload()) {
// 			if (response->ParseFromString(maybe_response.value().payload())) {
// 				if (response->SerializeAsString() == // TODO(lennart) topic specific? See subscribe
// 				    subscription_topic_.SerializeAsString()) { 
// 						notification_response_ = *response;
// 				}
// 			}
// 		}
// 	};

// 	// NotificationsRequest const unregister_notifications_request = buildUnregisterNotificationsRequest();
// 	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct

// 	rpc_handle_ =
// 	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

// 	// TODO(lennart) any handle for the response?

// 	done->Run();	
// }

// void RpcClientUSubscription::FetchSubscribers(
// 	google::protobuf::RpcController* controller,
// 	const ::uprotocol::core::usubscription::v3::FetchSubscribersRequest* request,
// 	::uprotocol::core::usubscription::v3::FetchSubscribersResponse* response,
// 	::google::protobuf::Closure* done) {
	
// 	constexpr int REQUEST_TTL_TIME = 0x8000; // TODO(lennart) time?
// 	constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIBERS = 0x0008;
// 	auto request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
// 	auto priority = uprotocol::v1::UPriority::UPRIORITY_UNSPECIFIED;
	
// 	rpc_client_ = std::make_unique<communication::RpcClient>(
// 	    transport_, uSubscriptionUUriBuilder_.getServiceUriWithResourceId(RESOURCE_ID_FETCH_SUBSCRIBERS),
// 	    priority, request_ttl);

// 	auto on_response = [this, response](const auto& maybe_response) {
// 		if (maybe_response.has_value() &&
// 		    maybe_response.value().has_payload()) {
// 			if (response->ParseFromString(maybe_response.value().payload())) {
// 				if (response->SerializeAsString() == // TODO(lennart) topic specific? See subscribe
// 				    subscription_topic_.SerializeAsString()) { 
// 						fetch_subscribers_response_ = *response;
// 				}
// 			}
// 		}
// 	};

// 	// FetchSubscribersRequest const fetch_subscribers_request = buildFetchSubscribersRequest();
// 	auto payload = datamodel::builder::Payload(*request); // TODO(lennart) check if request is correct

// 	rpc_handle_ =
// 	    rpc_client_->invokeMethod(std::move(payload), std::move(on_response));

// 	// TODO(lennart) any handle for the response?

// 	done->Run();	
// }

}  // namespace uprotocol::core::usubscription::v3
