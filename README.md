# uc_stripe
This is an Ubercart payment gateway module for the Stripe payment processor. 

The module from on drupal.org has the issue:
* a redundant code;
* not compliant with Drupal standards;
* doesn't use a web hook.

Therefore, this module was created. I created fork from version [7.3.0-rc2](https://www.drupal.org/project/uc_stripe/releases/7.x-3.0-rc2). 

This module uses Strong Customer Authentication (SCA) [more](https://stripe.com/en-ca/guides/strong-customer-authentication) and has a ready implemented [web-hook](https://stripe.com/docs/webhooks).

This module is used on 4 production sites.
