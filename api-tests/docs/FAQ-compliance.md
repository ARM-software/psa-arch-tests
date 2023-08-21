
# Frequently Asked Questions about PSA Certified API compliance


## Introduction

The Platform Security Architecture initiative defines a set of common APIs meant to level the field around secure services offered by a Root of Trust. Those APIs cover a minimal set of services that should be included into all devices like cryptography, secure storage, attestation, and firmware update.

Crypto vendors may also be interested in adopting just the Crypto API part for their crypto libraries or hardware.

PSA Certified delivers compliance badges for both device-level vendors who want to check their compliance agains the whole API set, and Crypto compliance badges for crypto vendors who want to check their compliance against all or a subset of the Crypto API.



### How should those APIs be referred to?

The official name for those APIs is `PSA Certified APIs`. API specifications are placed in [PSA API github site](https://github.com/ARM-software/psa-api/).
Other common ways to refer to those APIs are: `PSA APIs`, `PSA functional APIs`.
`PSA Certified Crypto API` is often shortened to `Crypto API` in documentation.

### What is PSA Certified API compliance? How does it relate to PSA certification?

[PSA Certification](https://www.psacertified.org) aims at grading the robustness of Root of Trust implementations.

PSA Certification does not mandate the implementation of the PSA Certified APIs. Root of Trust implementers are free to choose whatever interfaces they want to offer to their users, the certification process only looks at security properties and robustness offered by implementations.

PSA API compliance aims at measuring adoption for the APIs, by delivering compliance badges to implementers of those APIs. PSA API implementations do not need to be certified by PSA at any level to receive a compliance badge.

In summary: you can PSA-certify a product without implementing PSA Certified APIs, and you can implement PSA Certified APIs and seek the compliance badge without applying for PSA Certification.

### What is the PSA API compliance badge?

The general API compliance badge is meant for Device vendors who provide access to their Root of Trust services through PSA APIs: Crypto, Secure Storage, and Attestation.

For device vendors, obtaining the API compliance badge is a way to communicate to their customers that they can rely on the presence of standardized APIs to access Root of Trust services. The minimum requirement to pass general API compliance is to support:

- Enough Crypto APIs to support TF-M Profile Small, i.e. `crypto_hash`, `crypto_cipher`, and `crypto_rng`.
- At least one Secure Storage service: ITS (Internal Trusted Storage) or PS (Protected Storage), or both.
- The Attestation service API.

Once you get your badge, your product will be displayed on [PSA Certified products](https://www.psacertified.org/certified-products/).

### What is the Crypto API compliance badge?

The Crypto API compliance badge is meant for pure crypto players who provide access to their crypto software or hardware through PSA Certified Crypto APIs.
Product examples include open-source and commercial crypto libraries, crypto hardware accelerators, secure elements, smart cards, TPM equivalents, TrustZone-based services, and anything that offers high-level cryptography services like hash, signature, or encryption.

For pure crypto players, obtaining the Crypto API compliance badge is a way to communicate to their customers:

- Compliance with the Crypto API standard, implying no need for developers to re-learn yet another proprietary API.
- How many high-level crypto services are supported by a given crypto product.

Ideally, it should be possible for developers to determine at a glance whether a crypto engine (hardware or software) has enough functionality to implement their use cases. This is exactly what the Crypto API compliance badge is trying to achieve.


### Which high-level crypto services are included in the Crypto API compliance badge?

High-level crypto services describe the main operations offered by a crypto engine (hardware or software) like encrypt/decrypt, sign/verify, hash, generate random, and key derivation and agreement protocols. Most crypto use cases can be described by combining those high-level functions.

Announcing high-level crypto services is useful for developers looking for ways to develop cryptography-based use cases like secure network connections, secure storage, or authentication protocols. For example, establishing a handshake in a mutually-authenticated protocol may require key agreement, key generation, symmetric ciphers, and hash functions.


High-level Crypto services  | Crypto badge mention
--------------------------  | --------------------
Hash                        | `crypto_hash`
MAC                         | `crypto_mac`
Symmetric and stream ciphers            | `crypto_cipher`
Asymmetric signature and verification   | `crypto_asym_sign`
Asymmetric encryption/decryption        | `crypto_asym_crypt`
Authenticated Encryption with Associated Data   | `crypto_aead`
Key derivation              | `crypto_derivation`
Key agreement               | `crypto_agreement`
Random Number Generation    | `crypto_rng`

Functions that need to be implemented for each badge:

| Badge mention              | Functions
| -------------------------  | -----
| All badges                 | `init`<br>`import_key`<br>`export_key`<br>`destroy_key`<br>`key_attributes_set_get`<br>`generate_key`<br>`copy_key`
| `crypto_hash` | `hash_setup`<br>`hash_update`<br>`hash_verify`<br>`hash_finish`<br>`hash_abort`<br>`hash_clone`<br><br>optional:<br>`hash_compute`<br>`hash_compare`<br>`hash_suspend`<br>`hash_resume`
| `crypto_mac`  | `mac_sign_setup`<br>`mac_update`<br>`mac_sign_finish`<br>`mac_verify_setup`<br>`mac_verify_finish`<br>`mac_abort`<br><br>optional:<br>`mac_compute`<br>`mac_verify`
| `crypto_asym_sign` | `sign_hash`<br>`verify_hash`<br>`sign_message`<br>`verify_message`
| `crypto_asym_crypt`  | `asymmetric_encrypt`<br>`asymmetric_decrypt`
| `crypto_cipher` | `cipher_encrypt`<br>`cipher_decrypt`<br>`cipher_encrypt_setup`<br>`cipher_decrypt_setup`<br>`cipher_generate_iv`<br>`cipher_set_iv`<br>`cipher_update`<br>`cipher_finish`<br>`cipher_abort`
| `crypto_aead` | `aead_encrypt`<br>`aead_decrypt`<br>`aead_encrypt_setup`<br>`aead_decrypt_setup`<br>`aead_generate_nonce`<br>`aead_set_nonce`<br>`aead_set_lengths`<br>`aead_update_ad`<br>`aead_update`<br>`aead_finish`<br>`aead_abort`<br>`aead_verify`
| `crypto_derivation`  | `key_derivation_setup`<br>`key_derivation_input_bytes`<br>`key_derivation_input_key`<br>`key_derivation_key_agreement`<br>`key_derivation_output_bytes`<br>`key_derivation_output_key`<br>`key_derivation_abort`<br>`key_derivation_set_get_capacity`
| `crypto_agreement`   | `raw_key_agreement`
| `crypto_rng`         | `generate_random`

This list is probably not exhaustive. Crypto vendors are expected to use common sense to determine which functions of a given subset are interesting for their own users.

### What about crypto primitives like AES, RSA, SHA?

The API compliance badge only shows what kind of high-level crypto services are offered by compatible products. Low-level crypto primitives like AES, RSA, SHA, or specific elliptic curves are already always clearly announced by crypto vendors on their product datasheets. The API compliance badge is about functional compliance with a promise of portability, rather than about low-level crypto primitive support.

The Crypto API part of the compliance kit expects a minimal set of crypto primitives to be present to pass test cases. See test cases for more details about which primitives are tested.

### What is the process to obtain a PSA API compliance badge?

To pass general PSA API compliance tests you need to:

- Download the compliance kit from this repository.
- Possibly adapt the test suite to reflect which tests you want to pass or skip.
- Run the tests on a device that runs the Root of Trust you want to apply for.
- Send the logs to `psacertified@arm.com` and mention you want to apply for General PSA API compliance. You should pass at least enough Crypto API to satisfy a TF-M Profile Small use case, at least one secure storage service, and implement the (small) Attestation API. You can also apply for compliance without the Attestation API and the badge you will receive will mention exactly what tests you passed.

To pass Crypto API compliance tests you need to:

- Download the compliance kit from this repository.
- Possibly adapt the test suite to reflect which tests you want to pass or skip.
- Run the tests on any compatible platform..
- Send the logs to `psacertified@arm.com` and mention you want to apply for Crypto API compliance.

It is possible to apply for Crypto API compliance even when implementing very restricted subsets of Crypto. For example, a hardware hash accelerator that offers compatible hash APIs could apply and obtain a PSA Certified API compliance badge that would only mention `crypto_hash`.

### How much does it cost to obtain a PSA API compliance badge?

Nothing.

### How long does it take to obtain a PSA API compliance badge?

Delivering a badge usually takes a few working days, please bear with us if any delays occur.

--------------

*Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.*
