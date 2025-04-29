#ifndef USUBSCRIPTION_H
#define USUBSCRIPTION_H
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

namespace uprotocol::core::usubscription::v3 {

	struct USubscription {

		virtual ~USubscription() = default;

		virtual SubscriptionResponse subscribe(const SubscriptionRequest& subscription_request) = 0;

		virtual UnsubscribeResponse unsubscribe(const UnsubscribeRequest& unsubscribe_request) = 0;

		virtual FetchSubscriptionsResponse fetch_subscriptions(const FetchSubscriptionsRequest& fetch_subscribers_request) = 0;

		virtual NotificationsResponse register_for_notifications(const NotificationsRequest& register_notifications_request) =0 ;

		virtual NotificationsResponse unregister_for_notifications(const NotificationsRequest& unregister_notifications_request) = 0;

		virtual FetchSubscribersResponse fetch_subscribers(const FetchSubscribersRequest& fetch_subscribers_request) = 0;

	};

} // namespace uprotocol::core::usubscription::v3

#endif //USUBSCRIPTION_H
