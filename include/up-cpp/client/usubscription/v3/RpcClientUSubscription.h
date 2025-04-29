//
// Created by max on 28.04.25.
//

#ifndef RPCCLIENTUSUBSCRIPTION_H
#define RPCCLIENTUSUBSCRIPTION_H

#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include "USubscription.h"
#include "up-cpp/communication/RpcClient.h"

namespace uprotocol::core::usubscription::v3 {

struct RpcClientUSubscription : USubscription {

	explicit RpcClientUSubscription(std::unique_ptr<communication::RpcClient> client)
		: client_(std::move(client)){};

	void default_call_option();

	SubscriptionResponse subscribe(const SubscriptionRequest& subscription_request) override;

	UnsubscribeResponse unsubscribe(const UnsubscribeRequest& unsubscribe_request) override;

	FetchSubscriptionsResponse fetch_subscriptions(const FetchSubscriptionsRequest& fetch_subscribers_request) override;

	NotificationsResponse register_for_notifications(const NotificationsRequest& register_notifications_request) override;

	NotificationsResponse unregister_for_notifications(const NotificationsRequest& unregister_notifications_request) override;

	FetchSubscribersResponse fetch_subscribers(const FetchSubscribersRequest& fetch_subscribers_request) override;

private:
	std::unique_ptr<communication::RpcClient> client_;

};

} // namespace uprotocol::core::usubscription::v3

#endif //RPCCLIENTUSUBSCRIPTION_H
