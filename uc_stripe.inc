<?php

/**
 * @file It contains helpers for uc_stripe.
 */


/**
 * Loads the PHP API
 *
 * @return mixed
 *   Stripe version number as string or FALSE if failed to load
 */
function uc_stripe_load_library() {
  if (($library = libraries_load('stripe')) && !empty($library['loaded']) && class_exists('\Stripe\Stripe')) {
    return \Stripe\Stripe::VERSION;
  }
  watchdog('uc_stripe', 'Stripe PHP Library not installed or wrong version');
  return FALSE;

}

/**
 * Checks that all API keys are configured.
 *
 * @return bool
 *   TRUE if all 4 keys have a value.
 */
function uc_stripe_check_api_keys() {
  return (variable_get('uc_stripe_api_key_live_publishable', FALSE) &&
    variable_get('uc_stripe_api_key_live_secret', FALSE) &&
    variable_get('uc_stripe_api_key_test_publishable', FALSE) &&
    variable_get('uc_stripe_api_key_test_secret', FALSE));
}

/**
 * Sanitize and strip whitespace from Stripe keys
 *
 * @param string $key
 *
 * @return string
 */
function uc_stripe_sanitize_key($key) {
  $key = trim($key);
  $key = check_plain($key);
  return $key;
}

/**
 * Validates Stripe key
 *
 * @param string $key
 * @return boolean
 */
function uc_stripe_validate_key($key) {
  $valid = preg_match('/^[a-zA-Z0-9_]+$/', $key);
  return $valid;
}

/**
 * Retrieves the Stripe customer id for a user by given user ID.
 *
 * @param integer $uid.
 *   User ID.
 * @return mixed
 *   bool - if does not exist "uc_stripe_customer_id" in user then return FALSE
 *   string - if exist "uc_stripe_customer_id" in user then return this.
 */
function uc_stripe_get_customer_id($uid) {
  $account = user_load($uid);
  $id = !empty($account->data['uc_stripe_customer_id']) ? $account->data['uc_stripe_customer_id'] : FALSE;
  return $id;
}

/**
 * Retrieves the Stripe payment id for a user by given user ID.
 *
 * @param integer $uid.
 *   User ID.
 * @return mixed
 *   bool - if does not exist "uc_stripe_payment_id" in user then return FALSE
 *   string - if exist "uc_stripe_payment_id" in user then return this.
 */
function uc_stripe_get_payment_id($uid) {
  $account = user_load($uid);
  $id = !empty($account->data['uc_stripe_payment_id']) ? $account->data['uc_stripe_payment_id'] : FALSE;
  return $id;
}

/**
 * Returns Stripe's key which uses for API.
 *
 * @param string $type.
 *   'public' - then function will return public key.
 *   'secret' - then function will return secret key.
 *
 * @return string.
 */
function uc_stripe_get_key($type = 'public') {
  $buf = array(
    'public' => array(
      'test' => variable_get('uc_stripe_api_key_test_publishable', ''),
      'live' => variable_get('uc_stripe_api_key_live_publishable', ''),
    ),
    'secret' => array(
      'test' => variable_get('uc_stripe_api_key_test_secret', ''),
      'live' => variable_get('uc_stripe_api_key_live_secret', ''),
    ),
  );
  $mode = 'live';
  if (variable_get('uc_stripe_testmode', TRUE)) {
    $mode = 'test';
  }

  return $buf[$type][$mode];
}

/**
 * Used to return the appropriate response after checking Stripe Payment Intent
 * status
 * @param Object $intent
 * @return string response
 */
function uc_stripe_generate_payment_response($intent) {

  if ($intent->status == 'requires_action' &&
    $intent->next_action->type == 'use_stripe_sdk') {

    # Tell the client to handle the action
    $response = array(
      'requires_action' => TRUE,
      'payment_intent_client_secret' => $intent->client_secret
    );
  }
  elseif ($intent->status == 'succeeded') {
    # The payment didn’t need any additional actions and completed!
    # Handle post-payment fulfillment
    $response = array('success' => TRUE);
  }
  else {
    # Invalid status
    http_response_code(500);
    $response = array('error' => 'Invalid PaymentIntent status');
  }

  return $response;
}


function uc_stripe_confirm_payment_error($message) {
  $_SESSION['stripe']['payment_failed'] = TRUE;
  return array('error' => $message);
}

/**
 * This function returns the default off session authention email text.
 * @return $text - Email text
 */
function uc_stripe_get_authentication_required_email_text() {
  $text = t("Dear [user:name],

We were unable to process your subscription payment.

Your financial institution is requesting additional verification before your subscription can be renewed.

Please visit this link to return to our site and complete the verification step.

[uc_stripe:verification-link]

-- [site:name] team

");

  return $text;
}

/**
 *
 * Token callback that adds the authentication link to user mails.
 *
 * This function is used by the token_replace() call in uc_stripe_mail() to add
 * the url to verify payment information
 *
 * @param $replacements
 *   An associative array variable containing mappings from token names to
 *   values (for use with strtr()).
 * @param $data
 *   An associative array of token replacement values.
 * @param $options
 *   Unused parameter required by the token_replace() function.
 */
function uc_stripe_mail_tokens(&$replacements, $data, $options) {
  if (!empty($data['user']) && !empty($data['authentication_key'])) {
    $url = url(
      'user/' . $data['user']->uid . '/uc_stripe_authenticate_payment/' . $data['authentication_key'],
      array('absolute' => TRUE)
    );
    $replacements['[uc_stripe:verification-link]'] = $url;
  }
}