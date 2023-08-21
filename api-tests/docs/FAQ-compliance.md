
# Frequently Asked Questions about PSA Certified API compliance


## Introduction

The Platform Security Architecture initiative defines a set of common APIs meant to level the field around secure services offered by a root of trust. Those APIs cover a minimal set of services that should be included into all devices.


### How should those APIs be referred to?

The official name for those APIs is `PSA Certified APIs`. API specifications are placed in [PSA API github site](https://github.com/ARM-software/psa-api/).
Other common ways to refer to those APIs are: `PSA APIs`, `PSA functional APIs`.
`PSA Certified Crypto API` is often shortened to `Crypto API` in documentation.

### What is PSA Certified API compliance? How does it relate to PSA certification?

[PSA Certification](https://www.psacertified.org) aims at grading the robustness of root-of-trust implementations.

PSA Certification does not mandate the implementation of the PSA Certified APIs. Root of Trust implementers are free to choose whatever interfaces they want to offer to their users, the certification process only looks at security properties offered by implementations.

PSA API compliance aims at measuring adoption for the APIs, by delivering compliance badges to implementers. PSA API implementations do not need to be certified by PSA at any level to receive a compliance badge.

In summary: you can PSA-certify a product without implementing PSA Certified APIs, and you can implement PSA Certified APIs and seek the compliance badge without applying for PSA Certification.

### What is the PSA API compliance badge?

The general API compliance badge is meant for Device vendors who provide access to their Root of Trust services through PSA APIs: Crypto, Secure Storage, and Attestation.

For device vendors, obtaining the API compliance badge is a way to communicate to their customers that they can rely on the presence of standardized APIs to access Root of Trust services. The minimum requirement to pass the device API compliance is to support:

- Enough Crypto APIs to support TF-M Profile Small: `crypto_hash`, `crypto_cipher`, `crypto_rng`.
- At least one Secure Storage service: ITS (Internal Trusted Storage) or PS (Protected Storage), or both.
- The Attestation service API.

### What is the Crypto API compliance badge?

The Crypto API compliance badge is meant for pure crypto players who provide access to their crypto software or hardware through PSA Crypto APIs.
Product examples include open-source and commercial crypto libraries, crypto hardware accelerators, secure elements, smart cards, TPM equivalents, TrustZone-based services.

For pure crypto players, obtaining the Crypto API compliance badge is a way to communicate to their customers:

- Compliance with the Crypto API standard, no need for developers to re-learn yet another proprietary API.
- How many high-level crypto services are supported by a given crypto product.


### Which high-level crypto services are included in the Crypto API compliance badge?

High-level crypto services describe the main operations offered by a crypto engine (hardware or software) like encrypt/decrypt, sign/verify, hash, generate random, and key derivation and agreement protocols. Most crypto use cases can be described by combining some of those high-level functions.

Example: establishing a TLS session may involve key generation (including using a Random Number Generator), key agreement for the initial handshake, symmetric ciphers and hash/MAC functions for session encryption and integrity protection.


High-level Crypto services  | Crypto badge mention
--------------------------  | --------------------
Hash            | `crypto_hash`
MAC             | `crypto_mac`
Symmetric and stream ciphers            | `crypto_cipher`
Asymmetric signature and verification   | `crypto_asym_sign`
Asymmetric encryption/decryption        | `crypto_asym_crypt`
Authenticated Encryption with Associated Data   | `crypto_aead`
Key derivation  | `crypto_derivation`
Key agreement   | `crypto_agreement`
Random Number Generation                | `crypto_rng`



### What about crypto primitives like AES, RSA, SHA?

The API compliance badge only shows what kind of high-level crypto services are offered by compatible products. Low-level crypto primitives like hash functions, ciphers, or elliptic curves are already always clearly announced by crypto vendors on their product datasheets. The API compliance badge is about functional compliance with a promise of portability, rather than about low-level crypto primitive support.


Functions that need to be implemented for each badge:

| Badge mention              | Functions
| -------------------------  | -----
| Always required            | `init` `import_key` `export_key` `destroy_key` `key_attributes_set_get` `generate_key` `copy_key`
| Required for `crypto_hash` | `hash_setup` `hash_update` `hash_verify` `hash_finish` `hash_abort` `hash_clone` 
| Optional for `crypto_hash` | `hash_compute` `hash_compare` `hash_suspend` `hash_resume`
| Required for `crypto_mac`  | `mac_sign_setup` `mac_update` `mac_sign_finish` `mac_verify_setup` `mac_verify_finish` `mac_abort`
| Optional for `crypto_mac`  | `mac_compute` `mac_verify`
| Required for `crypto_asym_sign`   | `sign_hash` `verify_hash` `sign_message` `verify_message`
| Required for `crypto_asym_crypt`  | `asymmetric_encrypt` `asymmetric_decrypt`
| Required for `crypto_cipher`      | `cipher_encrypt` `cipher_decrypt` `cipher_encrypt_setup` `cipher_decrypt_setup` `cipher_generate_iv` `cipher_set_iv` `cipher_update` `cipher_finish` `cipher_abort`
| Required for `crypto_aead`        | `aead_encrypt` `aead_decrypt` `aead_encrypt_setup` `aead_decrypt_setup` `aead_generate_nonce` `aead_set_nonce` `aead_set_lengths` `aead_update_ad` `aead_update` `aead_finish` `aead_abort` `aead_verify`
| Required for `crypto_derivation`  | `key_derivation_setup` `key_derivation_input_bytes` `key_derivation_input_key` `key_derivation_key_agreement` `key_derivation_output_bytes` `key_derivation_output_key` `key_derivation_abort` `key_derivation_set_get_capacity`
| Required for `crypto_agreement`   | `raw_key_agreement`
| Required for `crypto_rng`         | `generate_random`

### Why apply for a PSA API compliance badge?

### Which APIs are needed for compliance?

### What is the process to obtain a PSA API compliance badge?

### How much does it cost to obtain a PSA API compliance badge?

### How long does it take to obtain a PSA API compliance badge?


--------------

*Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.*
