#ifndef RPCCLIENTUSUBSCRIPTION_H
#define RPCCLIENTUSUBSCRIPTION_H

#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include "Consumer.h"

// #include "USubscription.h"
#include "up-cpp/communication/RpcClient.h"

namespace uprotocol::core::usubscription::v3 {

struct RpcClientUSubscription : public uSubscription {
public:
	RpcClientUSubscription() = default;
	~RpcClientUSubscription() override = default;
	
	explicit RpcClientUSubscription(std::unique_ptr<communication::RpcClient> rpc_client)
		: rpc_client_(std::move(rpc_client)){};

	
	void Subscribe(google::protobuf::RpcController* controller,
					const ::uprotocol::core::usubscription::v3::SubscriptionRequest* request,
					::uprotocol::core::usubscription::v3::SubscriptionResponse* response,
					::google::protobuf::Closure* done) override;

	void Unsubscribe(google::protobuf::RpcController* controller,
						const ::uprotocol::core::usubscription::v3::UnsubscribeRequest* request,
						::uprotocol::core::usubscription::v3::UnsubscribeResponse* response,
						::google::protobuf::Closure* done) override;

	void FetchSubscriptions(google::protobuf::RpcController* controller,
									const ::uprotocol::core::usubscription::v3::FetchSubscriptionsRequest* request,
									::uprotocol::core::usubscription::v3::FetchSubscriptionsResponse* response,
									::google::protobuf::Closure* done) override;

	void RegisterForNotifications(google::protobuf::RpcController* controller,
											const ::uprotocol::core::usubscription::v3::NotificationsRequest* request,
											::uprotocol::core::usubscription::v3::NotificationsResponse* response,
											::google::protobuf::Closure* done) override;

	void UnregisterForNotifications(google::protobuf::RpcController* controller,
											const ::uprotocol::core::usubscription::v3::NotificationsRequest* request,
											::uprotocol::core::usubscription::v3::NotificationsResponse* response,
											::google::protobuf::Closure* done) override;

	void FetchSubscribers(google::protobuf::RpcController* controller,
									const ::uprotocol::core::usubscription::v3::FetchSubscribersRequest* request,
									::uprotocol::core::usubscription::v3::FetchSubscribersResponse* response,
									::google::protobuf::Closure* done) override;

	void Reset(google::protobuf::RpcController* controller,
						const ::uprotocol::core::usubscription::v3::ResetRequest* request,
						::uprotocol::core::usubscription::v3::ResetResponse* response,
						::google::protobuf::Closure* done) override;

// Backup
	void default_call_option();

	// SubscriptionResponse subscribe(const SubscriptionRequest& subscription_request);

	// UnsubscribeResponse unsubscribe(const UnsubscribeRequest& unsubscribe_request);

	// FetchSubscriptionsResponse fetch_subscriptions(const FetchSubscriptionsRequest& fetch_subscribers_request);

	// NotificationsResponse register_for_notifications(const NotificationsRequest& register_notifications_request);

	// NotificationsResponse unregister_for_notifications(const NotificationsRequest& unregister_notifications_request);

	// FetchSubscribersResponse fetch_subscribers(const FetchSubscribersRequest& fetch_subscribers_request);

private:
	// RPC request
	std::unique_ptr<communication::RpcClient> rpc_client_;
	communication::RpcClient::InvokeHandle rpc_handle_;
	SubscriptionResponse subscription_response_;
	
	// Transport
	std::shared_ptr<transport::UTransport> transport_;
	
	// URI info about the uSubscription service
	client::usubscription::v3::USubscriptionUUriBuilder uSubscriptionUUriBuilder_;
	
	// Topic to subscribe to
	const v1::UUri subscription_topic_;
};

} // namespace uprotocol::core::usubscription::v3

#endif //RPCCLIENTUSUBSCRIPTION_H
