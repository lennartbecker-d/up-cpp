#ifndef USUBSCRIPTION_H
#define USUBSCRIPTION_H
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/ustatus.pb.h>
#include "up-cpp/utils/Expected.h"

namespace uprotocol::core::usubscription::v3 {

	struct USubscription {

		template<typename R>
		using ResponseOrStatus = utils::Expected<R, v1::UStatus>;

		virtual ~USubscription() = default;

		virtual ResponseOrStatus<SubscriptionResponse> subscribe(const SubscriptionRequest& subscription_request) = 0;

		// virtual UnsubscribeResponse unsubscribe(const UnsubscribeRequest& unsubscribe_request) = 0;

		// virtual FetchSubscriptionsResponse fetch_subscriptions(const FetchSubscriptionsRequest& fetch_subscribers_request) = 0;

		// virtual NotificationsResponse register_for_notifications(const NotificationsRequest& register_notifications_request) =0 ;

		// virtual NotificationsResponse unregister_for_notifications(const NotificationsRequest& unregister_notifications_request) = 0;

		// virtual FetchSubscribersResponse fetch_subscribers(const FetchSubscribersRequest& fetch_subscribers_request) = 0;

	};

} // namespace uprotocol::core::usubscription::v3

#endif //USUBSCRIPTION_H
