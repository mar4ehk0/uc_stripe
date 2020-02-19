<?php

/**
 * @file
 * Hooks provided by the 'uc_stripe' module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Called after Stipe's subscription was cancelled.
 */
function hook_uc_stripe_subscription_canceled($order) {
  watchdog('module_name', 'Subscription for order #@order_id was cancelled',
    array('@order_id' => $order->order_id)
  );
}

/**
 * Called after Stipe's subscription was cancelled.
 */
function hook_uc_stripe_renew_failed($order, $fee) {
  watchdog('module_name', 'Subscription for order #@order_id was cancelled',
    array('@order_id' => $order->order_id)
  );
}

/**
 * @} End of "addtogroup hooks".
 */
