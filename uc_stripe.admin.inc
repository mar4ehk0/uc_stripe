<?php
/**
 * @file
 * Contains menu callback for settings form.
 */


/**
 * Provide configuration form for uc_stripe
 *
 * @return mixed
 */
function uc_stripe_settings_form() {
  $form['uc_stripe_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stripe settings'),
  );

  $form['uc_stripe_settings']['uc_stripe_api_key_test_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Test Secret Key'),
    '#default_value' => variable_get('uc_stripe_api_key_test_secret', ''),
    '#description' => t('Your Development Stripe API Key. Must be the "secret" key, not the "publishable" one.'),
  );

  $form['uc_stripe_settings']['uc_stripe_api_key_test_publishable'] = array(
    '#type' => 'textfield',
    '#title' => t('Test Publishable Key'),
    '#default_value' => variable_get('uc_stripe_api_key_test_publishable', ''),
    '#description' => t('Your Development Stripe API Key. Must be the "publishable" key, not the "secret" one.'),
  );

  $form['uc_stripe_settings']['uc_stripe_api_key_live_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Live Secret Key'),
    '#default_value' => variable_get('uc_stripe_api_key_live_secret', ''),
    '#description' => t('Your Live Stripe API Key. Must be the "secret" key, not the "publishable" one.'),
  );

  $form['uc_stripe_settings']['uc_stripe_api_key_live_publishable'] = array(
    '#type' => 'textfield',
    '#title' => t('Live Publishable Key'),
    '#default_value' => variable_get('uc_stripe_api_key_live_publishable', ''),
    '#description' => t('Your Live Stripe API Key. Must be the "publishable" key, not the "secret" one.'),
  );

  $form['uc_stripe_settings']['emails'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail of Stripe'),
  );
  $form['uc_stripe_settings']['emails']['uc_stripe_emails_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => variable_get('uc_stripe_emails_enabled', TRUE),
  );
  $subject = t('Additional Verification Required to Process Subscription.');
  $form['uc_stripe_settings']['emails']['uc_stripe_authentication_required_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject of Email for Recurring payment authentication'),
    '#default_value' => variable_get('uc_stripe_authentication_required_email_subject', $subject),
    '#states' => array(
      'invisible' => array(
        ':input[name="' . 'uc_stripe_emails_enabled"]' => array(
          'checked' => FALSE
        ),
      ),
    ),
  );
  $body = uc_stripe_get_authentication_required_email_text();
  $form['uc_stripe_settings']['emails']['uc_stripe_authentication_required_email_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body of Email for Recurring payment authentication'),
    '#default_value' => variable_get('uc_stripe_authentication_required_email', $body),
    '#description' => t('If your site uses recurring payments, some transactions will require the customer to return to the site and authenticate before the subscription payment can be processed.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="' . 'uc_stripe_emails_enabled"]' => array(
          'checked' => FALSE
        ),
      ),
    ),
  );

  $form['uc_stripe_settings']['uc_stripe_testmode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Test mode'),
    '#description' => 'Testing Mode: Stripe will use the development API key to process the transaction so the card will not actually be charged.',
    '#default_value' => variable_get('uc_stripe_testmode', TRUE),
  );

  $form['uc_stripe_settings']['uc_stripe_poweredby'] = array(
    '#type' => 'checkbox',
    '#title' => t('Powered by Stripe'),
    '#description' => 'Show "powered by Stripe" in checkout.',
    '#default_value' => variable_get('uc_stripe_poweredby', FALSE),
  );

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter() for uc_payment_method_settings_form().
 *
 * @see uc_payment_method_settings_form()
 *
 */
function uc_stripe_form_uc_payment_method_settings_form_alter(&$form, &$form_state) {
  $form['#validate'][] = 'uc_stripe_settings_form_validate';
}

/**
 * Validation function and normalize keys (trim spaces)
 */
function uc_stripe_settings_form_validate($form, &$form_state) {
  $elements = array(
    'uc_stripe_api_key_test_secret',
    'uc_stripe_api_key_test_publishable',
    'uc_stripe_api_key_live_secret',
    'uc_stripe_api_key_live_publishable'
  );

  if ($form_state['values']['uc_pg_uc_stripe_enabled']) {
    foreach ($elements as $element_name) {
      $form_state['values'][$element_name] = uc_stripe_sanitize_key($form_state['values'][$element_name]);
      if (!uc_stripe_validate_key($form_state['values'][$element_name])) {
        form_set_error($element_name, t('@name does not appear to be a valid stripe key', array('@name' => $element_name)));
      }
    }
  }

  // Make sure they haven't tried to validate credit card numbers, as uc_stripe will not provide a real one.
  if (!empty($form_state['values']['uc_credit_validate_numbers'])) {
    form_set_error('uc_credit_validate_numbers', t('When used with Ubercart Stripe, "Validate credit card number at checkout" must be unchecked.'));
  }

}