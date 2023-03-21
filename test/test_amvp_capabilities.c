/** @file */
/*
 * Copyright (c) 2021, Cisco Systems, Inc.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/cisco/libamvp/LICENSE
 */


#include "ut_common.h"

AMVP_CTX *ctx;
char cvalue[] = "same";
AMVP_RESULT rv;

static void teardown(void) {
    if (ctx) teardown_ctx(&ctx);
}

Test(EnableCapHash, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);

    rv = amvp_cap_hash_enable(ctx, AMVP_HASH_SHA1, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65528, 8);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65532, 4);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65534, 2);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65535, 1);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * This test should return AMVP_NO_CAP because we are trying
 * to register a parameter for an alg that we haven't added
 * to the list yet.
 */
Test(EnableCapHash, param_alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hash_enable(ctx, AMVP_HASH_SHA1, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    
    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA256, AMVP_HASH_MESSAGE_LEN,
                                  0, 65535, 1);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * Attempts to register with a NULL handler
 */
Test(EnableCapHash, null_handler, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hash_enable(ctx, AMVP_HASH_SHA1, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Tests invalid values to enable_hash_cap_parm API
 */
Test(EnableCapHash, invalid_args, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hash_enable(ctx, AMVP_HASH_SHA1, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65535, 2);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65535, 4);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_hash_set_domain(ctx, AMVP_HASH_SHA1, AMVP_HASH_MESSAGE_LEN,
                                  0, 65535, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Tests a good kdf108 api sequence
 */
Test(EnableCapKDF108, good, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf108_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF108, AMVP_PREREQ_HMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_domain(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTED_LEN, 8, 384, 8);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_domain(ctx, AMVP_KDF108_MODE_FEEDBACK, AMVP_KDF108_SUPPORTED_LEN, 8, 384, 8);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_domain(ctx, AMVP_KDF108_MODE_DPI, AMVP_KDF108_SUPPORTED_LEN, 8, 384, 8);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_HMAC_SHA1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_HMAC_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_HMAC_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_HMAC_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_HMAC_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_CMAC_AES128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_CMAC_AES192);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_CMAC_AES256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_CMAC_TDES);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_COUNTER_LEN, 8);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_FIXED_DATA_ORDER, AMVP_KDF108_FIXED_DATA_ORDER_AFTER);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_FIXED_DATA_ORDER, AMVP_KDF108_FIXED_DATA_ORDER_BEFORE);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_FIXED_DATA_ORDER, AMVP_KDF108_FIXED_DATA_ORDER_MIDDLE);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_FIXED_DATA_ORDER, AMVP_KDF108_FIXED_DATA_ORDER_NONE);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_FIXED_DATA_ORDER, AMVP_KDF108_FIXED_DATA_ORDER_BEFORE_ITERATOR);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTS_EMPTY_IV, 0);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * This tests invalid kdf108 mode
 */
Test(EnableCapKDF108, alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf108_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_domain(ctx, 0, AMVP_KDF108_SUPPORTED_LEN, 8, 384, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_parm(ctx, 999, AMVP_KDF108_MAC_MODE, AMVP_KDF108_MAC_MODE_HMAC_SHA1);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * This tests invalid params to kdf108_domain_param API
 */
Test(EnableCapKDF108, invalid_domain, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf108_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_domain(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTED_LEN, 0, 384, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_domain(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTED_LEN, 8, 99999, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Tests invalid values to the kdf108_cap_param API
 */
Test(EnableCapKDF108, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf108_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_MAC_MODE, 999);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_COUNTER_LEN, 7);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_COUNTER_LEN, 999);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_FIXED_DATA_ORDER, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTS_EMPTY_IV, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTS_EMPTY_IV, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTS_EMPTY_IV, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf108_set_parm(ctx, AMVP_KDF108_MODE_COUNTER, AMVP_KDF108_SUPPORTS_EMPTY_IV, 2);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapKDFx963, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    /* Enable capabilites */
    rv = amvp_cap_kdf135_x963_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_X963, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_KEY_DATA_LEN, 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_KEY_DATA_LEN, 1024);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_FIELD_SIZE, 224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_FIELD_SIZE, 521);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_SHARED_INFO_LEN, 0);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_SHARED_INFO_LEN, 1024);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * tries to enable kdf x963 with empty ctx, expect fail
 */
Test(EnableCapKDFx963, null_ctx, .fini = teardown) {
    rv = amvp_cap_kdf135_x963_enable(NULL, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * tries to enable kdf x963 with invalid params, expect fail
 */
Test(EnableCapKDFx963, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);

    // shouldn't be called before enable_cap
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_NO_CAP);
    rv = amvp_cap_kdf135_x963_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_HASH_ALG, 999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_KEY_DATA_LEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_KEY_DATA_LEN, 99999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_FIELD_SIZE, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_FIELD_SIZE, 99999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_SHARED_INFO_LEN, -1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_x963_set_parm(ctx, AMVP_KDF_X963_SHARED_INFO_LEN, 9999);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapKDFSNMP, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_snmp_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_SNMP, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_snmp_set_parm(ctx, AMVP_KDF135_SNMP, AMVP_KDF135_SNMP_PASS_LEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_snmp_set_parm(ctx, AMVP_KDF135_SNMP, AMVP_KDF135_SNMP_PASS_LEN, 64);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_snmp_set_engid(ctx, AMVP_KDF135_SNMP, "0a0a0a0a0a0a0a0a0a");
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * This test should return AMVP_NO_CAP because we are trying
 * to register a parameter for an alg that we haven't added
 * to the list yet.
 */
Test(EnableCapKDFSNMP, param_alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_snmp_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_SSH, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * This test gives invalid params to kdf_snmp api
 */
Test(EnableCapKDFSNMP, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_snmp_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_snmp_set_parm(ctx, AMVP_KDF135_SNMP, AMVP_KDF135_SNMP_PASS_LEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_snmp_set_parm(ctx, AMVP_KDF135_SNMP, AMVP_KDF135_SNMP_PASS_LEN, 99999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_snmp_set_engid(ctx, AMVP_KDF135_SNMP, "0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a");
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Good srtp registration
 */
Test(EnableCapKDFSRTP, good, .fini = teardown) {
    int i = 0;
    
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_srtp_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_SRTP, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_SUPPORT_ZERO_KDR, 0);
    cr_assert(rv == AMVP_SUCCESS);
    for (i = 0; i < 24; i++) {
        rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_KDF_EXPONENT, i + 1);
        cr_assert(rv == AMVP_SUCCESS);
    }
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_AES_KEYLEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_AES_KEYLEN, 192);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_AES_KEYLEN, 256);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable srtp with null ctx
 */
Test(EnableCapKDFSRTP, null_ctx, .fini = teardown) {
    rv = amvp_cap_kdf135_srtp_enable(NULL, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * enable srtp with invalid params, expect fail
 */
Test(EnableCapKDFSRTP, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_srtp_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_SUPPORT_ZERO_KDR, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_KDF_EXPONENT, -1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_KDF_EXPONENT, 25);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_srtp_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SRTP_AES_KEYLEN, 512);
    cr_assert(rv == AMVP_INVALID_ARG);
}


Test(EnableCapKDFSSH, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ssh_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_SSH, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_SSH, AMVP_PREREQ_TDES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_SSH, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SSH, AMVP_SSH_METH_TDES_CBC, AMVP_SHA256 | AMVP_SHA384 | AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SSH, AMVP_SSH_METH_AES_128_CBC, AMVP_SHA256 | AMVP_SHA384 | AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SSH, AMVP_SSH_METH_AES_192_CBC, AMVP_SHA256 | AMVP_SHA384 | AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SSH, AMVP_SSH_METH_AES_256_CBC, AMVP_SHA256 | AMVP_SHA384 | AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * tries to enable kdf ssh with null_ctx, expect failure
 */
Test(EnableCapKDFSSH, null_ctx, .fini = teardown) {
    rv = amvp_cap_kdf135_ssh_enable(NULL, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * tries to enable kdf ssh with invalid params, expect failure
 */
Test(EnableCapKDFSSH, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ssh_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SSH, AMVP_SSH_METH_TDES_CBC, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * This test should return AMVP_NO_CAP because we are trying
 * to register a parameter for an alg that we haven't added
 * to the list yet.
 */
Test(EnableCapKDFSSH, param_alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ssh_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SNMP, AMVP_SSH_METH_TDES_CBC, AMVP_SHA256 | AMVP_SHA384 | AMVP_SHA512);
    cr_assert(rv == AMVP_NO_CAP);
    rv = amvp_cap_kdf135_ssh_set_parm(ctx, AMVP_KDF135_SRTP, AMVP_SSH_METH_TDES_CBC, AMVP_SHA256 | AMVP_SHA384 | AMVP_SHA512);
    cr_assert(rv == AMVP_NO_CAP);
}

Test(EnableCapCMAC, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_cmac_enable(ctx, AMVP_CMAC_AES, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_CMAC_AES, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_cmac_set_domain(ctx, AMVP_CMAC_AES, AMVP_CMAC_MSGLEN, 0, 65536, 8);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_MACLEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_KEYLEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_DIRECTION_GEN, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_DIRECTION_VER, 1);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * This test should return AMVP_NO_CAP because we are trying
 * to register a parameter for an alg that we haven't added
 * to the list yet.
 */
Test(EnableCapCMAC, param_alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_cmac_enable(ctx, AMVP_CMAC_AES, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_cmac_set_domain(ctx, AMVP_CMAC_TDES, AMVP_CMAC_MSGLEN, 0, 65536, 8);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * Attempts to register with a NULL handler
 */
Test(EnableCapCMAC, null_handler, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_cmac_enable(ctx, AMVP_CMAC_AES, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapCMAC, invalid_args, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_cmac_enable(ctx, AMVP_CMAC_AES, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_cmac_enable(ctx, AMVP_CMAC_TDES, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    
    rv = amvp_cap_set_prereq(ctx, AMVP_CMAC_AES, AMVP_PREREQ_AES, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_domain(ctx, AMVP_CMAC_AES, AMVP_CMAC_MSGLEN, -1, 65536, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_domain(ctx, AMVP_CMAC_AES, AMVP_CMAC_MSGLEN, 0, 9999999, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_domain(ctx, AMVP_CMAC_AES, AMVP_CMAC_MSGLEN, 0, 65536, 7);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    /*
     * CMAC-AES has different keylen requirements than other MACs
     * 128, 192, 256 are allowed
     */
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_KEYLEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_KEYLEN, 191);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_KEYLEN, 512);
    cr_assert(rv == AMVP_INVALID_ARG);
    /* the rest are a range */
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_TDES, AMVP_CMAC_KEYLEN, 7);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_TDES, AMVP_CMAC_KEYLEN, 524289);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_MACLEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_KEYLEN, 513);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    /* Only applicable to TDES */
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_KEYING_OPTION, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_TDES, AMVP_CMAC_KEYING_OPTION, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_TDES, AMVP_CMAC_KEYING_OPTION, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    /* these are flags... 0 or 1 */
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_DIRECTION_GEN, -1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_DIRECTION_GEN, 2);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_DIRECTION_VER, -1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_cmac_set_parm(ctx, AMVP_CMAC_AES, AMVP_CMAC_DIRECTION_VER, 2);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapHMAC, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hmac_enable(ctx, AMVP_HMAC_SHA2_224, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_HMAC_SHA2_224, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_hmac_set_domain(ctx, AMVP_HMAC_SHA2_224, AMVP_HMAC_KEYLEN, 256, 448, 8);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_hmac_set_domain(ctx, AMVP_HMAC_SHA2_224, AMVP_HMAC_MACLEN, 32, 224, 8);
    cr_assert(rv == AMVP_SUCCESS);
}

Test(EnableCapHMAC, param_alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hmac_enable(ctx, AMVP_HMAC_SHA2_256, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    
    rv = amvp_cap_hmac_enable(ctx, AMVP_CMAC_AES, &dummy_handler_success);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_hmac_set_parm(ctx, AMVP_HMAC_SHA2_224, AMVP_HMAC_KEYLEN, 32 * 8);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * Attempts to register with a NULL handler
 */
Test(EnableCapHMAC, null_handler, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hmac_enable(ctx, AMVP_HMAC_SHA2_384, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapHMAC, invalid_args, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_hmac_enable(ctx, AMVP_HMAC_SHA2_512, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    
    rv = amvp_cap_set_prereq(ctx, AMVP_HMAC_SHA2_512, AMVP_PREREQ_SHA, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_hmac_set_parm(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_KEYLEN, 7);
    cr_assert(rv == AMVP_INVALID_ARG);
    /*
     * TODO: need to make sure the library checks if the max is greater than the min
     * [edaw] the domain here needs refactoring
     */
    rv = amvp_cap_hmac_set_parm(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_KEYLEN, 524889);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_hmac_set_domain(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_KEYLEN, 7, 256, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_hmac_set_domain(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_KEYLEN, 8, 524889, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    rv = amvp_cap_hmac_set_parm(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_MACLEN, 31);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_hmac_set_parm(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_MACLEN, 513);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_hmac_set_domain(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_MACLEN, 31, 512, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_hmac_set_domain(ctx, AMVP_HMAC_SHA2_512, AMVP_HMAC_MACLEN, 32, 513, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapRSAkeyGen, proper_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_RSA_KEYGEN, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_RSA_KEYGEN, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_parm(ctx, AMVP_RSA_PARM_PUB_EXP_MODE, AMVP_RSA_PUB_EXP_MODE_FIXED);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_parm(ctx, AMVP_RSA_PARM_INFO_GEN_BY_SERVER, 1);
    cr_assert(rv == AMVP_SUCCESS);
}

Test(EnableCapRSAkeyGen, proper_modes, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    /* skip the other outer params for now */
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B32);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B33);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B34);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B35);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B36);
    cr_assert(rv == AMVP_SUCCESS);
}

Test(EnableCapRSAkeyGen, proper_modes_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    /* skip the other outer params for now */
    
    /* B.3.5 takes both a hash_alg and a prime_test
     * so we can test with that one... */
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B35);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_primes(ctx, AMVP_RSA_KEYGEN_B35, 2048, AMVP_RSA_PRIME_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_primes(ctx, AMVP_RSA_KEYGEN_B35, 2048, AMVP_RSA_PRIME_TEST, AMVP_RSA_PRIME_TEST_TBLC2);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_primes(ctx, AMVP_RSA_KEYGEN_B35, 3072, AMVP_RSA_PRIME_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_primes(ctx, AMVP_RSA_KEYGEN_B35, 3072, AMVP_RSA_PRIME_TEST, AMVP_RSA_PRIME_TEST_TBLC2);
    cr_assert(rv == AMVP_SUCCESS);
}

Test(EnableCapRSAkeyGen, alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_HASH_SHA1, &dummy_handler_success);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_set_prereq(ctx, AMVP_RSA_KEYGEN, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * Most of these params are members of enums, so the app
 * won't even build if it has an invalid value
 */
Test(EnableCapRSAkeyGen, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_set_prereq(ctx, AMVP_RSA_KEYGEN, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_NO_CAP);
    
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_RSA_KEYGEN, AMVP_PREREQ_SHA, "");
    cr_assert(rv == AMVP_INVALID_ARG);
    
    /* should only accept true or false... 0 or 1 */
    rv = amvp_cap_rsa_keygen_set_parm(ctx, AMVP_RSA_PARM_INFO_GEN_BY_SERVER, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    /* should only accept true or false... 0 or 1 */
    rv = amvp_cap_rsa_keygen_set_parm(ctx, AMVP_RSA_PARM_KEY_FORMAT_CRT, 2);
    cr_assert(rv == AMVP_INVALID_ARG);
}

Test(EnableCapRSAkeyGen, invalid_modes_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    /* skip the other outer params for now */
    
    /* B.3.5 takes both a hash_alg and a prime_test
     * so we can test with that one... */
    rv = amvp_cap_rsa_keygen_set_mode(ctx, AMVP_RSA_KEYGEN_B35);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_primes(ctx, AMVP_RSA_KEYGEN_B35, 2048, AMVP_RSA_PRIME_HASH_ALG, 257);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_rsa_keygen_set_primes(ctx, AMVP_RSA_KEYGEN_B35, 2048, AMVP_RSA_PRIME_TEST, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Enable an AES cipher mode, then attempt to register
 * a parameter for a different mode
 */
Test(EnableCapRSAkeyGen, cipher_param_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_rsa_keygen_enable(ctx, AMVP_RSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_rsa_keygen_set_parm(ctx, AMVP_RSA_PARM_RAND_PQ, 128);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    /*
     * Interesting here:
     * Commented out as an example (doesn't compile)
     *
     *   rv = amvp_cap_rsa_keygen_set_parm(ctx, AMVP_SYM_CIPH_PTLEN, 128);
     *
     * if the compiler lets you compile with the wrong enum type here,
     * if that value has a valid value in the expected enum then the
     * test may not fail. The compiler flag to error on implicit casting
     * is -Werror=enum-conversion but we can't control what an app
     * on the outside compiles with...
     */
    
}

Test(EnableCapAES, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_GCM, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_AES_GCM, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_AES_GCM, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PARM_DIR, AMVP_SYM_CIPH_DIR_BOTH);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PARM_KO, AMVP_SYM_CIPH_KO_NA);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PARM_IVGEN_SRC, AMVP_SYM_CIPH_IVGEN_SRC_INT);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PARM_IVGEN_MODE, AMVP_SYM_CIPH_IVGEN_MODE_821);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_KEYLEN, 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_TAGLEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_IVLEN, 96);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PTLEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_AADLEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
}

Test(EnableCapAES, alg_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_ECDSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_set_prereq(ctx, AMVP_AES_GCM, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_NO_CAP);
}

Test(EnableCapAES, bad_conformance, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CTR, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CTR, AMVP_SYM_CIPH_PARM_CONFORMANCE, AMVP_CONFORMANCE_DEFAULT);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_GCM, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PARM_CONFORMANCE, AMVP_CONFORMANCE_RFC3686);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Most of these params are members of enums, so the app
 * won't even build if it has an invalid value
 */
Test(EnableCapAES, invalid_callback, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_GCM, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_set_prereq(ctx, AMVP_AES_GCM, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * Most of these params are members of enums, so the app
 * won't even build if it has an invalid value
 */
Test(EnableCapAES, invalid_dir, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_GCM, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_GCM, AMVP_SYM_CIPH_PARM_DIR, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Enable an AES cipher mode, then attempt to register
 * a parameter for a different mode
 */
Test(EnableCapAES, cipher_param_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_GCM, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_KEYLEN, 128);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * Enable an AES cipher mode, then attempt to register
 * a parameter for a different mode
 */
Test(EnableCapAES, invalid_keylens, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CFB1, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);

    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CFB1, AMVP_SYM_CIPH_KEYLEN, 333);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CFB1, AMVP_SYM_CIPH_KEYLEN, 333333);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CFB1, AMVP_SYM_CIPH_KEYLEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Enable an AES cipher mode, then attempt to register
 * a parameter for a different mode
 */
Test(EnableCapAES, invalid_param_lens, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CFB1, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CFB1, AMVP_SYM_CIPH_PTLEN, 333333);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CCM, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_TAGLEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_TAGLEN, 333);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_IVLEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_IVLEN, 333333);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_IVLEN, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_AADLEN, 333333);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CCM, AMVP_SYM_CIPH_AADLEN, -333);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * Enable an AES cipher mode, then attempt to register
 * a domain for a non-domain value 
 */
Test(EnableCapAES, cipher_invalid_parm_domain, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CBC_CS2, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_domain(ctx, AMVP_AES_CBC_CS2, AMVP_SYM_CIPH_KEYLEN, 0, 128, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * null CTX set domain
 */
Test(EnableCapAES, cipher_domain_no_ctx, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_XPN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_domain(NULL, AMVP_AES_XPN, AMVP_SYM_CIPH_DOMAIN_PTLEN, 0, 128, 8);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * bad domain values
 */
Test(EnableCapAES, cipher_domain_bad_values, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CBC_CS3, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_domain(ctx, AMVP_AES_CBC_CS3, AMVP_SYM_CIPH_DOMAIN_PTLEN, -64, 128, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_domain(ctx, AMVP_AES_CBC_CS3, AMVP_SYM_CIPH_DOMAIN_PTLEN, 128, 64, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_domain(ctx, AMVP_AES_CBC_CS3, AMVP_SYM_CIPH_DOMAIN_PTLEN, 0, 128, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_domain(ctx, AMVP_AES_CBC_CS3, AMVP_SYM_CIPH_KEYLEN, 0, 0, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/* 
 * Enable an AES cipher, register a payloadLen, then register a payloadLen domain
 */
Test(EnableCapAES, dup_payload_registration, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_AES_CBC_CS1, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_AES_CBC_CS1, AMVP_SYM_CIPH_PTLEN, 33333);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_domain(ctx, AMVP_AES_CBC_CS1, AMVP_SYM_CIPH_DOMAIN_PTLEN, 0, 1024, 8);
    cr_assert(rv == AMVP_INVALID_ARG);

}


Test(EnableCapTDES, properly, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_TDES_CBC, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_TDES_CBC, AMVP_SYM_CIPH_PARM_DIR, AMVP_SYM_CIPH_DIR_BOTH);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_TDES_CBC, AMVP_SYM_CIPH_PARM_KO, AMVP_SYM_CIPH_KO_ONE);
    cr_assert(rv == AMVP_SUCCESS);
}

Test(EnableCapTDES, alg_param_mismatch, .fini = teardown) {
    setup_empty_ctx(&ctx);
    rv = amvp_cap_sym_cipher_enable(ctx, AMVP_TDES_CBC, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_TDES_CBC, AMVP_SYM_CIPH_TAGLEN, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_sym_cipher_set_parm(ctx, AMVP_TDES_CBC, AMVP_SYM_CIPH_AADLEN, 64);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * This calls enable ikev1 api properly
 */
Test(EnableCapKDF135IKEv1, good, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev1_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_IKEV1, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_IKEV1, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_INIT_NONCE_LEN, 64, 2048, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_RESPOND_NONCE_LEN, 64, 2048, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_DH_SECRET_LEN, 224, 8192, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_PSK_LEN, 8, 8192, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, AMVP_SHA1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_AUTH_METHOD, AMVP_KDF135_IKEV1_AMETH_PSK);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_AUTH_METHOD, AMVP_KDF135_IKEV1_AMETH_DSA);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_AUTH_METHOD, AMVP_KDF135_IKEV1_AMETH_PKE);
    cr_assert(rv == AMVP_SUCCESS);
}


/*
 * This calls enable ikev1 api with null values
 * TODO this is another case that might be allowed
 * when running in non-realtime
 */
Test(EnableCapKDF135IKEv1, null_vals, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev1_enable(NULL, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
    rv = amvp_cap_kdf135_ikev1_enable(ctx, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * This calls enable ikev1 domain api with invalid params
 */
Test(EnableCapKDF135IKEv1, invalid_domain, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev1_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_INIT_NONCE_LEN, 0, 2048, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_INIT_NONCE_LEN, 64, 9999, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_RESPOND_NONCE_LEN, 0, 2048, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_RESPOND_NONCE_LEN, 64, 9999, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_DH_SECRET_LEN, 0, 8192, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_DH_SECRET_LEN, 224, 99999, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_PSK_LEN, 0, 8192, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_domain(ctx, AMVP_KDF_IKEv1_PSK_LEN, 8, 99999, 1);
    cr_assert(rv == AMVP_INVALID_ARG);
}


/*
 * This calls enable ikev1 param api with invalid params
 */
Test(EnableCapKDF135IKEv1, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, 999);
    cr_assert(rv == AMVP_NO_CAP);
    rv = amvp_cap_kdf135_ikev1_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_HASH_ALG, 999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev1_set_parm(ctx, AMVP_KDF_IKEv1_AUTH_METHOD, 0);
    cr_assert(rv == AMVP_INVALID_ARG);

}

/*
 * This calls enable ikev2 api properly
 */
Test(EnableCapKDF135IKEv2, good, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev2_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_IKEV2, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF135_IKEV2, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_INIT_NONCE_LEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_INIT_NONCE_LEN, 2048);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_RESPOND_NONCE_LEN, 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_RESPOND_NONCE_LEN, 2048);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_DH_SECRET_LEN, 2048);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_KEY_MATERIAL_LEN, 1056);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_KEY_MATERIAL_LEN, 3072);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_parm(ctx, AMVP_KDF_HASH_ALG, AMVP_SHA1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_parm(ctx, AMVP_KDF_HASH_ALG, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_parm(ctx, AMVP_KDF_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_parm(ctx, AMVP_KDF_HASH_ALG, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_parm(ctx, AMVP_KDF_HASH_ALG, AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * This calls enable ikev2 domain api properly
 */
Test(EnableCapKDF135IKEv2, good_domain, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev2_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_RESPOND_NONCE_LEN, 64, 2048, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_INIT_NONCE_LEN, 64, 2048, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_DH_SECRET_LEN, 224, 8192, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_KEY_MATERIAL_LEN, 160, 2048, 1);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * This calls enable ikev2 api properly
 */
Test(EnableCapKDF135IKEv2, null_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev2_enable(NULL, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
    rv = amvp_cap_kdf135_ikev2_enable(ctx, NULL);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * This calls enable ikev2 len/domain api with invalid params
 */
Test(EnableCapKDF135IKEv2, invalid_len_params, .fini = teardown) {
    setup_empty_ctx(&ctx);

    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_INIT_NONCE_LEN, 9999);
    cr_assert(rv == AMVP_NO_CAP);
    rv = amvp_cap_kdf135_ikev2_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_INIT_NONCE_LEN, 9999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_RESPOND_NONCE_LEN, 9999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_DH_SECRET_LEN, 9999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_length(ctx, AMVP_KEY_MATERIAL_LEN, 99999);
    cr_assert(rv == AMVP_INVALID_ARG);

    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_INIT_NONCE_LEN, 0, 256, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_INIT_NONCE_LEN, 64, 9999, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_RESPOND_NONCE_LEN, 0, 256, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_RESPOND_NONCE_LEN, 64, 9999, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_DH_SECRET_LEN, 0, 256, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_DH_SECRET_LEN, 224, 9999, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_KEY_MATERIAL_LEN, 0, 256, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf135_ikev2_set_domain(ctx, AMVP_KEY_MATERIAL_LEN, 224, 99999, 8);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * This calls enable ikev2 api properly
 */
Test(EnableCapKDF135IKEv2, invalid_hash_alg, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf135_ikev2_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf135_ikev2_set_parm(ctx, AMVP_KDF_HASH_ALG, 999);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * enable ecdsa keygen with valid params
 */
Test(EnableCapECDSA, good_keygen, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_KEYGEN, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_KEYGEN, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P521);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_SECRET_GEN, AMVP_ECDSA_SECRET_GEN_TEST_CAND);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable ecdsa cipher mismatch
 */
Test(EnableCapECDSA, mode_mismatch_kg, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_KDF135_SNMP, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B233);
    cr_assert(rv == AMVP_NO_CAP);
}

/*
 * enable ecdsa keygen with invalid params
 */
Test(EnableCapECDSA, invalid_params_kg, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_KEYGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_CURVE, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_SECRET_GEN, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYGEN, AMVP_ECDSA_SECRET_GEN, 0);
    cr_assert(rv == AMVP_MISSING_ARG);
}



/*
 * enable ecdsa keyver with valid params
 */
Test(EnableCapECDSA, good_keyver, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_KEYVER, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_KEYVER, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_KEYVER, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P521);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B571);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable ecdsa keyver with invalid params
 */
Test(EnableCapECDSA, invalid_params_kv, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_KEYVER, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_KEYVER, AMVP_ECDSA_CURVE, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
}


/*
 * enable ecdsa siggen with valid params
 */
Test(EnableCapECDSA, good_siggen, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_SIGGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_SIGGEN, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_SIGGEN, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P521);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_HASH_ALG, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    //rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_HASH_ALG, AMVP_SHA384);
    rv = amvp_cap_ecdsa_set_curve_hash_alg(ctx, AMVP_ECDSA_SIGGEN, AMVP_EC_CURVE_B409, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_HASH_ALG, AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
}


/*
 * enable ecdsa siggen with invalid params
 */
Test(EnableCapECDSA, invalid_args_sg, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_SIGGEN, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_HASH_ALG, 257);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_ecdsa_set_curve_hash_alg(ctx, AMVP_ECDSA_SIGGEN, AMVP_EC_CURVE_K233, AMVP_SHA224);
    cr_assert(rv == AMVP_UNSUPPORTED_OP);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P192);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_curve_hash_alg(ctx, AMVP_ECDSA_SIGGEN, AMVP_EC_CURVE_P192, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * enable ecdsa sigver with valid params
 */
Test(EnableCapECDSA, good_sigver, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_SIGVER, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_SIGVER, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_ECDSA_SIGVER, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P521);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_K571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_B571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_HASH_ALG, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_HASH_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    //rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_HASH_ALG, AMVP_SHA384);
    rv = amvp_cap_ecdsa_set_curve_hash_alg(ctx, AMVP_ECDSA_SIGVER, AMVP_EC_CURVE_B409, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_HASH_ALG, AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable ecdsa sigver with invalid params
 */
Test(EnableCapECDSA, invalid_args_sv, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_ecdsa_enable(ctx, AMVP_ECDSA_SIGVER, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, 256);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_HASH_ALG, 257);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_ecdsa_set_curve_hash_alg(ctx, AMVP_ECDSA_SIGVER, AMVP_EC_CURVE_K233, AMVP_SHA224);
    cr_assert(rv == AMVP_UNSUPPORTED_OP);
    rv = amvp_cap_ecdsa_set_parm(ctx, AMVP_ECDSA_SIGVER, AMVP_ECDSA_CURVE, AMVP_EC_CURVE_P192);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_ecdsa_set_curve_hash_alg(ctx, AMVP_ECDSA_SIGVER, AMVP_EC_CURVE_P192, 3);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * enable hash drbg with valid params
 */
Test(EnableCapDRBG, good_hash, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_drbg_enable(ctx, AMVP_HASHDRBG, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                   AMVP_DRBG_DER_FUNC_ENABLED, 0);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_HASHDRBG, 
                                     AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                   AMVP_DRBG_PRED_RESIST_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                   AMVP_DRBG_RESEED_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                     AMVP_DRBG_ENTROPY_LEN, (int)128, (int)64,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                     AMVP_DRBG_NONCE_LEN, (int)96, (int)32,(int) 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                     AMVP_DRBG_PERSO_LEN, (int)0, (int)128,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                     AMVP_DRBG_ADD_IN_LEN, (int)0, (int)128,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HASHDRBG, AMVP_DRBG_SHA_1, 0,
                                   AMVP_DRBG_RET_BITS_LEN, 160);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable hmac drbg with valid params
 */
Test(EnableCapDRBG, good_hmac, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_drbg_enable(ctx, AMVP_HMACDRBG, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_HMACDRBG, 
                                     AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);    
    rv = amvp_cap_set_prereq(ctx, AMVP_HMACDRBG, 
                                     AMVP_PREREQ_HMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                   AMVP_DRBG_DER_FUNC_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                   AMVP_DRBG_PRED_RESIST_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                   AMVP_DRBG_RESEED_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                   AMVP_DRBG_RET_BITS_LEN, 224);
    cr_assert(rv == AMVP_SUCCESS);
    //Add length range
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                     AMVP_DRBG_ENTROPY_LEN, (int)192, (int)64,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                     AMVP_DRBG_NONCE_LEN, (int)192, (int)64,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                     AMVP_DRBG_PERSO_LEN, (int)0, (int)128,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_HMACDRBG, AMVP_DRBG_SHA_224, 0,
                                     AMVP_DRBG_ADD_IN_LEN, (int)0, (int)128,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable ctr drbg with valid params
 */
Test(EnableCapDRBG, good_ctr, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_drbg_enable(ctx, AMVP_CTRDRBG, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_CTRDRBG, 
                                     AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_CTRDRBG, 
                                     AMVP_PREREQ_TDES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_CTRDRBG, 
                                     AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                     AMVP_DRBG_ENTROPY_LEN, (int)128, (int)128, (int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                     AMVP_DRBG_NONCE_LEN, (int)64, (int)64,(int) 128);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                     AMVP_DRBG_PERSO_LEN, (int)0, (int)256,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_length(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                     AMVP_DRBG_ADD_IN_LEN, (int)0, (int)256,(int) 256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                   AMVP_DRBG_DER_FUNC_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                   AMVP_DRBG_PRED_RESIST_ENABLED, 1);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                   AMVP_DRBG_RESEED_ENABLED, 0);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_drbg_set_parm(ctx, AMVP_CTRDRBG, AMVP_DRBG_AES_128, 0,
                                   AMVP_DRBG_RET_BITS_LEN, 256);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable drbg with null ctx
 */
Test(EnableCapDRBG, null_ctx, .fini = teardown) {
    rv = amvp_cap_drbg_enable(NULL, AMVP_HASHDRBG, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * enable kas ecc with valid params
 */
Test(EnableCapKASECC, good, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kas_ecc_enable(ctx, AMVP_KAS_ECC_CDH, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_PREREQ_ECDSA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_FUNCTION, AMVP_KAS_ECC_FUNC_PARTIAL);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_P224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_P256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_P384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_P521);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_K233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_K283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_K409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_K571);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_B233);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_B283);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_B409);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_B571);
    cr_assert(rv == AMVP_SUCCESS);
    
    rv = amvp_cap_kas_ecc_enable(ctx, AMVP_KAS_ECC_COMP, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_PREREQ_ECDSA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_PREREQ_CCM, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_PREREQ_CMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_prereq(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_PREREQ_HMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_FUNCTION, AMVP_KAS_ECC_FUNC_PARTIAL);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED,  AMVP_KAS_ECC_ROLE, 0, AMVP_KAS_ECC_ROLE_INITIATOR);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED,  AMVP_KAS_ECC_ROLE, 0, AMVP_KAS_ECC_ROLE_RESPONDER);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED,  AMVP_KAS_ECC_KDF, 0, AMVP_KAS_ECC_NOKDFNOKC);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED, AMVP_KAS_ECC_EB, AMVP_EC_CURVE_P224, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED, AMVP_KAS_ECC_EC, AMVP_EC_CURVE_P256, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED, AMVP_KAS_ECC_ED, AMVP_EC_CURVE_P384, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED, AMVP_KAS_ECC_EE, AMVP_EC_CURVE_P521, AMVP_SHA512);
    cr_assert(rv == AMVP_SUCCESS);

}

/*
 * enable kas ecc with valid params
 */
Test(EnableCapKASECC, null_ctx, .fini = teardown) {
    rv = amvp_cap_kas_ecc_enable(NULL, AMVP_KAS_ECC_CDH, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * enable kas ecc with invalid
 */
Test(EnableCapKASECC, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kas_ecc_enable(ctx, AMVP_KAS_ECC_CDH, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_FUNCTION, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid cipher
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_AES_CBC, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_P224);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid mode
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, 0, AMVP_KAS_ECC_CURVE, AMVP_EC_CURVE_P256);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, 999);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_CDH, AMVP_KAS_ECC_MODE_CDH, AMVP_KAS_ECC_CURVE, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    rv = amvp_cap_kas_ecc_enable(ctx, AMVP_KAS_ECC_COMP, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ecc_set_parm(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_FUNCTION, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    
    // invalid cipher
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_RSA_KEYGEN, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED,  AMVP_KAS_ECC_ROLE, 0, AMVP_KAS_ECC_ROLE_INITIATOR);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid mode
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, 0, AMVP_KAS_ECC_EPHEMERAL_UNIFIED,  AMVP_KAS_ECC_ROLE, 0, AMVP_KAS_ECC_ROLE_RESPONDER);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid kdf set
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED,  AMVP_KAS_ECC_KDF, 0, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid scheme
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, 0, AMVP_KAS_ECC_EB, AMVP_EC_CURVE_P224, AMVP_SHA224);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid set
    rv = amvp_cap_kas_ecc_set_scheme(ctx, AMVP_KAS_ECC_COMP, AMVP_KAS_ECC_MODE_COMPONENT, AMVP_KAS_ECC_EPHEMERAL_UNIFIED, 0, AMVP_EC_CURVE_P256, AMVP_SHA256);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * enable kas ffc with valid params
 */
Test(EnableCapKASFFC, good, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kas_ffc_enable(ctx, AMVP_KAS_FFC_COMP, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_prereq(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_PREREQ_DSA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_prereq(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_PREREQ_SHA, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_prereq(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_PREREQ_DRBG, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_prereq(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_PREREQ_CCM, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_prereq(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_PREREQ_CMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_prereq(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_PREREQ_HMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_parm(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_FUNCTION, AMVP_KAS_FFC_FUNC_DPGEN);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_parm(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_FUNCTION, AMVP_KAS_FFC_FUNC_DPVAL);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL,  AMVP_KAS_FFC_ROLE, AMVP_KAS_FFC_ROLE_INITIATOR);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL,  AMVP_KAS_FFC_ROLE, AMVP_KAS_FFC_ROLE_RESPONDER);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL,  AMVP_KAS_FFC_KDF, AMVP_KAS_FFC_NOKDFNOKC);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL, AMVP_KAS_FFC_FB, AMVP_SHA224);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL, AMVP_KAS_FFC_FC, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL, AMVP_KAS_FFC_FB, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
}

/*
 * enable kas ffc with valid params
 */
Test(EnableCapKASFFC, null_ctx, .fini = teardown) {
    rv = amvp_cap_kas_ffc_enable(NULL, AMVP_KAS_FFC_COMP, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
}

/*
 * enable kas ffc with invalid params
 */
Test(EnableCapKASFFC, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kas_ffc_enable(ctx, AMVP_KAS_FFC_COMP, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    // invalid cipher
    rv = amvp_cap_kas_ffc_set_parm(ctx, AMVP_ECDSA_SIGGEN, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_FUNCTION, AMVP_KAS_FFC_FUNC_DPGEN);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid mode
    rv = amvp_cap_kas_ffc_set_parm(ctx, AMVP_KAS_FFC_COMP, 0, AMVP_KAS_FFC_FUNCTION, AMVP_KAS_FFC_FUNC_DPVAL);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid role
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, AMVP_KAS_FFC_DH_EPHEMERAL, AMVP_KAS_FFC_ROLE, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    // invalid scheme
    rv = amvp_cap_kas_ffc_set_scheme(ctx, AMVP_KAS_FFC_COMP, AMVP_KAS_FFC_MODE_COMPONENT, 0, AMVP_KAS_FFC_ROLE, AMVP_KAS_FFC_ROLE_RESPONDER);
    cr_assert(rv == AMVP_INVALID_ARG);
}

/*
 * enable kda hkdf with invalid params
 */
Test(EnableCapKASHKDF, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);

    rv = amvp_cap_kda_enable(ctx, AMVP_KDA_HKDF, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    // invalid cipher
    rv = amvp_cap_kda_set_parm(ctx, AMVP_HASH_SHA256, AMVP_KDA_PATTERN, AMVP_KDA_PATTERN_UPARTYINFO, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid pattern
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_PATTERN, AMVP_KDA_PATTERN_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid hmac alg
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_MAC_ALG, AMVP_HASH_ALG_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid l
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_L, 0, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid salt method
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_MAC_SALT, AMVP_KDA_MAC_SALT_METHOD_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid encoding
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_ENCODING_TYPE, AMVP_KDA_ENCODING_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid call to set Z on set parm
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_Z, 1024, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid call to set AUX_FUNCTION on hkdf
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_HKDF, AMVP_KDA_ONESTEP_AUX_FUNCTION, AMVP_HASH_SHA256, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid alg calls to set_domain
    rv = amvp_cap_kda_set_domain(ctx, AMVP_HASH_SHA256, AMVP_KDA_Z, 0, 4096, 8);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid param call to set_domain
    rv = amvp_cap_kda_set_domain(ctx, AMVP_KDA_HKDF, AMVP_KDA_L, 0, 4096, 8);
    cr_assert(rv = AMVP_INVALID_ARG);

}

/*
 * enable kda onestep with invalid params
 */
Test(EnableCapKASKDFONESTEP, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);

    rv = amvp_cap_kda_enable(ctx, AMVP_KDA_ONESTEP, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    // invalid cipher
    rv = amvp_cap_kda_set_parm(ctx, AMVP_HASH_SHA256, AMVP_KDA_PATTERN, AMVP_KDA_PATTERN_UPARTYINFO, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid pattern
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_PATTERN, AMVP_KDA_PATTERN_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid aux function
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_AES_GCM, AMVP_HASH_ALG_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid l
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_L, 0, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid salt method
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_MAC_SALT, AMVP_KDA_MAC_SALT_METHOD_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid encoding
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_ENCODING_TYPE, AMVP_KDA_ENCODING_MAX, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid call to set Z on set parm
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_Z, 1024, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid call to set HMAC_ALG on hkdf
    rv = amvp_cap_kda_set_parm(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_MAC_ALG, AMVP_HASH_SHA256, NULL);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid alg calls to set_domain
    rv = amvp_cap_kda_set_domain(ctx, AMVP_HASH_SHA256, AMVP_KDA_Z, 0, 4096, 8);
    cr_assert(rv = AMVP_INVALID_ARG);
    //invalid param call to set_domain
    rv = amvp_cap_kda_set_domain(ctx, AMVP_KDA_ONESTEP, AMVP_KDA_L, 0, 4096, 8);
    cr_assert(rv = AMVP_INVALID_ARG);
}

 /*
  * enable kdf tls 1.3 with valid parms
  */
Test(EnableCapKDFTLS13, valid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);

    rv = amvp_cap_kdf_tls13_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF_TLS13, AMVP_PREREQ_HMAC, cvalue);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_HMAC_ALG, AMVP_SHA256);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_HMAC_ALG, AMVP_SHA384);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_RUNNING_MODE, AMVP_KDF_TLS13_RUN_MODE_PSK);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_RUNNING_MODE, AMVP_KDF_TLS13_RUN_MODE_DHE);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_RUNNING_MODE, AMVP_KDF_TLS13_RUN_MODE_PSK_DHE);
    cr_assert(rv == AMVP_SUCCESS);
}

 /*
  * enable kdf tls 1.3 with invalid parms
  */
Test(EnableCapKDFTLS13, invalid_params, .fini = teardown) {
    setup_empty_ctx(&ctx);
    
    rv = amvp_cap_kdf_tls13_enable(NULL, &dummy_handler_success);
    cr_assert(rv == AMVP_NO_CTX);
    rv = amvp_cap_kdf_tls13_enable(ctx, &dummy_handler_success);
    cr_assert(rv == AMVP_SUCCESS);
    rv = amvp_cap_set_prereq(ctx, AMVP_KDF_TLS13, AMVP_PREREQ_AES, cvalue);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_HMAC_ALG, AMVP_SHA224);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, AMVP_KDF_TLS13_RUNNING_MODE, 0);
    cr_assert(rv == AMVP_INVALID_ARG);
    rv = amvp_cap_kdf_tls13_set_parm(ctx, 0, AMVP_SHA256);
    cr_assert(rv == AMVP_INVALID_ARG);
}