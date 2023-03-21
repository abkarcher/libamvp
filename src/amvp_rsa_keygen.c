/** @file */
/*
 * Copyright (c) 2019, Cisco Systems, Inc.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/cisco/libamvp/LICENSE
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "amvp.h"
#include "amvp_lcl.h"
#include "parson.h"
#include "safe_lib.h"

/*
 * After the test case has been processed by the DUT, the results
 * need to be JSON formated to be included in the vector set results
 * file that will be uploaded to the server.  This routine handles
 * the JSON processing for a single test case.
 */
static AMVP_RESULT amvp_rsa_output_tc(AMVP_CTX *ctx, AMVP_RSA_KEYGEN_TC *stc, JSON_Object *tc_rsp) {
    AMVP_RESULT rv = AMVP_SUCCESS;
    char *tmp = NULL;

    if (stc->rand_pq == AMVP_RSA_KEYGEN_B33 && stc->test_type == AMVP_RSA_TESTTYPE_KAT) {
        json_object_set_boolean(tc_rsp, "testPassed", stc->test_disposition);
        goto err;
    }

    tmp = calloc(AMVP_RSA_EXP_LEN_MAX + 1, sizeof(char));
    if (!tmp) {
        AMVP_LOG_ERR("Unable to malloc in amvp_kdf135 tpm_output_tc");
        return AMVP_MALLOC_FAIL;
    }

    rv = amvp_bin_to_hexstr(stc->p, stc->p_len, tmp, AMVP_RSA_EXP_LEN_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (p)");
        goto err;
    }
    json_object_set_string(tc_rsp, "p", (const char *)tmp);
    memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

    rv = amvp_bin_to_hexstr(stc->q, stc->q_len, tmp, AMVP_RSA_EXP_LEN_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (q)");
        goto err;
    }
    json_object_set_string(tc_rsp, "q", (const char *)tmp);
    memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

    rv = amvp_bin_to_hexstr(stc->n, stc->n_len, tmp, AMVP_RSA_EXP_LEN_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (n)");
        goto err;
    }
    json_object_set_string(tc_rsp, "n", (const char *)tmp);
    memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

    rv = amvp_bin_to_hexstr(stc->d, stc->d_len, tmp, AMVP_RSA_EXP_LEN_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (d)");
        goto err;
    }
    json_object_set_string(tc_rsp, "d", (const char *)tmp);
    memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

    rv = amvp_bin_to_hexstr(stc->e, stc->e_len, tmp, AMVP_RSA_EXP_LEN_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (e)");
        goto err;
    }
    json_object_set_string(tc_rsp, "e", (const char *)tmp);

    if (stc->key_format == AMVP_RSA_KEY_FORMAT_CRT) {
        rv = amvp_bin_to_hexstr(stc->xp, stc->xp_len, tmp, AMVP_RSA_EXP_LEN_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("hex conversion failure (xp)");
            goto err;
        }
        json_object_set_string(tc_rsp, "xP", (const char *)tmp);
        memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

        rv = amvp_bin_to_hexstr(stc->xp1, stc->xp1_len, tmp, AMVP_RSA_EXP_LEN_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("hex conversion failure (xp1)");
            goto err;
        }
        json_object_set_string(tc_rsp, "xP1", (const char *)tmp);
        memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

        rv = amvp_bin_to_hexstr(stc->xp2, stc->xp2_len, tmp, AMVP_RSA_EXP_LEN_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("hex conversion failure (xp2)");
            goto err;
        }
        json_object_set_string(tc_rsp, "xP2", (const char *)tmp);
        memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

        rv = amvp_bin_to_hexstr(stc->xq, stc->xq_len, tmp, AMVP_RSA_EXP_LEN_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("hex conversion failure (xq)");
            goto err;
        }
        json_object_set_string(tc_rsp, "xQ", (const char *)tmp);
        memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

        rv = amvp_bin_to_hexstr(stc->xq1, stc->xq1_len, tmp, AMVP_RSA_EXP_LEN_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("hex conversion failure (xq1)");
            goto err;
        }
        json_object_set_string(tc_rsp, "xQ1", (const char *)tmp);
        memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

        rv = amvp_bin_to_hexstr(stc->xq2, stc->xq2_len, tmp, AMVP_RSA_EXP_LEN_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("hex conversion failure (xq2)");
            goto err;
        }
        json_object_set_string(tc_rsp, "xQ2", (const char *)tmp);
        memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);
    }

    if (stc->info_gen_by_server) {
        if (stc->rand_pq == AMVP_RSA_KEYGEN_B33 ||
            stc->rand_pq == AMVP_RSA_KEYGEN_B35 ||
            stc->rand_pq == AMVP_RSA_KEYGEN_B36) {
            json_object_set_string(tc_rsp, "primeResult", (const char *)stc->prime_result);
        }
    } else {
        if (!(stc->rand_pq == AMVP_RSA_KEYGEN_B33)) {
            rv = amvp_bin_to_hexstr(stc->seed, stc->seed_len, tmp, AMVP_RSA_SEEDLEN_MAX);
            if (rv != AMVP_SUCCESS) {
                AMVP_LOG_ERR("hex conversion failure (seed)");
                goto err;
            }
            json_object_set_string(tc_rsp, "seed", (const char *)tmp);
            memzero_s(tmp, AMVP_RSA_EXP_LEN_MAX);

            json_object_set_value(tc_rsp, "bitlens", json_value_init_array());
            JSON_Array *bitlens_array = json_object_get_array(tc_rsp, "bitlens");
            json_array_append_number(bitlens_array, stc->bitlen1);
            json_array_append_number(bitlens_array, stc->bitlen2);
            json_array_append_number(bitlens_array, stc->bitlen3);
            json_array_append_number(bitlens_array, stc->bitlen4);
        }
    }

err:
    if (tmp) free(tmp);

    return rv;
}

/*
 * This function simply releases the data associated with
 * a test case.
 */

static AMVP_RESULT amvp_rsa_keygen_release_tc(AMVP_RSA_KEYGEN_TC *stc) {
    if (stc->e) { free(stc->e); }
    if (stc->seed) { free(stc->seed); }
    if (stc->p) { free(stc->p); }
    if (stc->q) { free(stc->q); }
    if (stc->n) { free(stc->n); }
    if (stc->d) { free(stc->d); }
    if (stc->xp1) { free(stc->xp1); }
    if (stc->xp2) { free(stc->xp2); }
    if (stc->xp) { free(stc->xp); }
    if (stc->xq1) { free(stc->xq1); }
    if (stc->xq2) { free(stc->xq2); }
    if (stc->xq) { free(stc->xq); }

    memzero_s(stc, sizeof(AMVP_RSA_KEYGEN_TC));

    return AMVP_SUCCESS;
}

static AMVP_RESULT amvp_rsa_keygen_init_tc(AMVP_CTX *ctx,
                                           AMVP_RSA_KEYGEN_TC *stc,
                                           unsigned int tc_id,
                                           AMVP_RSA_TESTTYPE test_type,
                                           int info_gen_by_server,
                                           AMVP_HASH_ALG hash_alg,
                                           AMVP_RSA_KEY_FORMAT key_format,
                                           AMVP_RSA_PUB_EXP_MODE pub_exp_mode,
                                           int modulo,
                                           AMVP_RSA_PRIME_TEST_TYPE prime_test,
                                           int rand_pq,
                                           const char *e,
                                           const char *p,
                                           const char *q,
                                           const char *xp,
                                           const char *xp1,
                                           const char *xp2,
                                           const char *xq,
                                           const char *xq1,
                                           const char *xq2,
                                           const char *seed,
                                           int seed_len,
                                           int bitlen1,
                                           int bitlen2,
                                           int bitlen3,
                                           int bitlen4) {
    memzero_s(stc, sizeof(AMVP_RSA_KEYGEN_TC));
    AMVP_RESULT rv = AMVP_SUCCESS;
    stc->test_type = test_type;
    stc->info_gen_by_server = info_gen_by_server;
    stc->tc_id = tc_id;
    stc->rand_pq = rand_pq;
    stc->modulo = modulo;
    stc->prime_test = prime_test;
    stc->hash_alg = hash_alg;
    stc->pub_exp_mode = pub_exp_mode;
    stc->key_format = key_format;

    stc->e = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
    if (!stc->e) { return AMVP_MALLOC_FAIL; }
    stc->p = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
    if (!stc->p) { return AMVP_MALLOC_FAIL; }
    stc->q = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
    if (!stc->q) { return AMVP_MALLOC_FAIL; }
    stc->n = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
    if (!stc->n) { return AMVP_MALLOC_FAIL; }
    stc->d = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
    if (!stc->d) { return AMVP_MALLOC_FAIL; }
    if (rand_pq == AMVP_RSA_KEYGEN_B36 || !info_gen_by_server) {
        stc->xp = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
        if (!stc->xp) { return AMVP_MALLOC_FAIL; }
        stc->xp1 = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
        if (!stc->xp1) { return AMVP_MALLOC_FAIL; }
        stc->xp2 = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
        if (!stc->xp2) { return AMVP_MALLOC_FAIL; }
        stc->xq = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
        if (!stc->xq) { return AMVP_MALLOC_FAIL; }
        stc->xq1 = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
        if (!stc->xq1) { return AMVP_MALLOC_FAIL; }
        stc->xq2 = calloc(AMVP_RSA_EXP_BYTE_MAX, sizeof(unsigned char));
        if (!stc->xq2) { return AMVP_MALLOC_FAIL; }
    }
    if (rand_pq == AMVP_RSA_KEYGEN_B36) {
        rv = amvp_hexstr_to_bin(xp, stc->xp, AMVP_RSA_EXP_BYTE_MAX, &(stc->xp_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (xp)");
            return rv;
        }
        rv = amvp_hexstr_to_bin(xp1, stc->xp1, AMVP_RSA_EXP_BYTE_MAX, &(stc->xp1_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (xp1)");
            return rv;
        }
        rv = amvp_hexstr_to_bin(xp2, stc->xp2, AMVP_RSA_EXP_BYTE_MAX, &(stc->xp2_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (xp2)");
            return rv;
        }
        rv = amvp_hexstr_to_bin(xq, stc->xq, AMVP_RSA_EXP_BYTE_MAX, &(stc->xq_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (xq)");
            return rv;
        }
        rv = amvp_hexstr_to_bin(xq1, stc->xq1, AMVP_RSA_EXP_BYTE_MAX, &(stc->xq1_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (xq1)");
            return rv;
        }
        rv = amvp_hexstr_to_bin(xq2, stc->xq2, AMVP_RSA_EXP_BYTE_MAX, &(stc->xq2_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (xq2)");
            return rv;
        }
    }
    if (p) {
        rv = amvp_hexstr_to_bin(p, stc->p, AMVP_RSA_EXP_BYTE_MAX, &(stc->p_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (p)");
            return rv;
        }
    }
    if (q) {
        rv = amvp_hexstr_to_bin(q, stc->q, AMVP_RSA_EXP_BYTE_MAX, &(stc->q_len));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (q)");
            return rv;
        }
    }

    rv = amvp_hexstr_to_bin(e, stc->e, AMVP_RSA_EXP_BYTE_MAX, &(stc->e_len));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (e)");
        return rv;
    }

    stc->seed = calloc(AMVP_RSA_SEEDLEN_MAX, sizeof(unsigned char));
    if (!stc->seed) { return AMVP_MALLOC_FAIL; }

    if (info_gen_by_server) {
        stc->bitlen1 = bitlen1;
        stc->bitlen2 = bitlen2;
        stc->bitlen3 = bitlen3;
        stc->bitlen4 = bitlen4;
        if (seed) {
            rv = amvp_hexstr_to_bin(seed, stc->seed, seed_len, &(stc->seed_len));
            if (rv != AMVP_SUCCESS) {
                return rv;
            }
        }
    }

    return AMVP_SUCCESS;
}

static AMVP_RSA_TESTTYPE read_test_type(const char *str) {
    int diff = 1;
    strcmp_s(AMVP_TESTTYPE_STR_KAT, sizeof(AMVP_TESTTYPE_STR_KAT) - 1, str, &diff);
    if (!diff) return AMVP_RSA_TESTTYPE_KAT;
    strcmp_s(AMVP_TESTTYPE_STR_AFT, sizeof(AMVP_TESTTYPE_STR_AFT) - 1, str, &diff);
    if (!diff) return AMVP_RSA_TESTTYPE_AFT;
    strcmp_s(AMVP_TESTTYPE_STR_GDT, sizeof(AMVP_TESTTYPE_STR_GDT) - 1, str, &diff);
    if (!diff) return AMVP_RSA_TESTTYPE_GDT;

    return 0;
}

static AMVP_RSA_PUB_EXP_MODE read_pub_exp_mode(const char *str){
    int diff = 1;

    strcmp_s(AMVP_RSA_PUB_EXP_MODE_FIXED_STR,
             AMVP_RSA_PUB_EXP_MODE_FIXED_STR_LEN, str, &diff);
    if (!diff) return AMVP_RSA_PUB_EXP_MODE_FIXED;

    strcmp_s(AMVP_RSA_PUB_EXP_MODE_RANDOM_STR,
             AMVP_RSA_PUB_EXP_MODE_RANDOM_STR_LEN, str, &diff);
    if (!diff) return AMVP_RSA_PUB_EXP_MODE_RANDOM;

    return 0;
}

static AMVP_RSA_KEY_FORMAT read_key_format(const char *str){
    int diff = 1;

    strcmp_s(AMVP_RSA_KEY_FORMAT_STD_STR,
             AMVP_RSA_KEY_FORMAT_STD_STR_LEN, str, &diff);
    if (!diff) return AMVP_RSA_KEY_FORMAT_STANDARD;

    strcmp_s(AMVP_RSA_KEY_FORMAT_CRT_STR,
             AMVP_RSA_KEY_FORMAT_CRT_STR_LEN, str, &diff);
    if (!diff) return AMVP_RSA_KEY_FORMAT_CRT;

    return 0;
}

static AMVP_RSA_PRIME_TEST_TYPE read_prime_test_type(const char *str) {
    int diff = 1;

    strcmp_s(AMVP_RSA_PRIME_TEST_TBLC2_STR,
             AMVP_RSA_PRIME_TEST_TBLC2_STR_LEN, str, &diff);
    if (!diff) return AMVP_RSA_PRIME_TEST_TBLC2;

    strcmp_s(AMVP_RSA_PRIME_TEST_TBLC3_STR,
             AMVP_RSA_PRIME_TEST_TBLC3_STR_LEN, str, &diff);
    if (!diff) return AMVP_RSA_PRIME_TEST_TBLC3;

    return 0;
}

AMVP_RESULT amvp_rsa_keygen_kat_handler(AMVP_CTX *ctx, JSON_Object *obj) {
    unsigned int tc_id;
    JSON_Value *groupval;
    JSON_Object *groupobj = NULL;
    JSON_Value *testval;
    JSON_Object *testobj = NULL;
    JSON_Array *groups;
    JSON_Array *tests;
    JSON_Array *bitlens;

    JSON_Value *reg_arry_val = NULL;
    JSON_Object *reg_obj = NULL;
    JSON_Array *reg_arry = NULL;

    int i, g_cnt;
    int j, t_cnt;

    JSON_Value *r_vs_val = NULL;
    JSON_Object *r_vs = NULL;
    JSON_Array *r_tarr = NULL, *r_garr = NULL;  /* Response testarray, grouparray */
    JSON_Value *r_tval = NULL, *r_gval = NULL;  /* Response testval, groupval */
    JSON_Object *r_tobj = NULL, *r_gobj = NULL; /* Response testobj, groupobj */
    AMVP_CAPS_LIST *cap;
    AMVP_RSA_KEYGEN_TC stc;
    AMVP_TEST_CASE tc;
    AMVP_RESULT rv;

    AMVP_CIPHER alg_id;
    char *json_result = NULL;
    unsigned int mod = 0;
    int info_gen_by_server, rand_pq, seed_len = 0;
    AMVP_HASH_ALG hash_alg = 0;
    AMVP_RSA_TESTTYPE test_type = 0;
    AMVP_RSA_PRIME_TEST_TYPE prime_test = 0;
    AMVP_RSA_PUB_EXP_MODE pub_exp_mode = 0;
    AMVP_RSA_KEY_FORMAT key_format = 0;
    const char *e_str = NULL, *p_str = NULL, *q_str = NULL, *xp_str = NULL, *xp1_str = NULL, 
               *xp2_str = NULL, *xq_str = NULL, *xq1_str = NULL, *xq2_str = NULL, *alg_str = NULL, 
               *mode_str = NULL, *hash_alg_str = NULL, *seed = NULL, *pub_exp_mode_str = NULL, 
               *key_format_str = NULL, *rand_pq_str = NULL, *prime_test_str = NULL,
               *test_type_str = NULL;
    int bitlen1 = 0, bitlen2 = 0, bitlen3 = 0, bitlen4 = 0;

    if (!ctx) {
        AMVP_LOG_ERR("No ctx for handler operation");
        return AMVP_NO_CTX;
    }

    alg_str = json_object_get_string(obj, "algorithm");
    if (!alg_str) {
        AMVP_LOG_ERR("Unable to parse 'algorithm' from JSON.");
        return AMVP_MALFORMED_JSON;
    }

    mode_str = json_object_get_string(obj, "mode");
    if (!mode_str) {
        AMVP_LOG_ERR("Unable to parse 'mode' from JSON.");
        return AMVP_MALFORMED_JSON;
    }

    alg_id = amvp_lookup_cipher_w_mode_index(alg_str, mode_str);
    if (alg_id != AMVP_RSA_KEYGEN) {
        AMVP_LOG_ERR("Server JSON invalid 'algorithm' or 'mode'");
        return AMVP_INVALID_ARG;
    }

    tc.tc.rsa_keygen = &stc;
    memzero_s(&stc, sizeof(AMVP_RSA_KEYGEN_TC));

    cap = amvp_locate_cap_entry(ctx, alg_id);
    if (!cap) {
        AMVP_LOG_ERR("Server requesting unsupported capability");
        return AMVP_UNSUPPORTED_OP;
    }

    /*
     * Create AMVP array for response
     */
    rv = amvp_create_array(&reg_obj, &reg_arry_val, &reg_arry);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("ERROR: Failed to create JSON response struct. ");
        return rv;
    }

    /*
     * Start to build the JSON response
     */
    rv = amvp_setup_json_rsp_group(&ctx, &reg_arry_val, &r_vs_val, &r_vs, alg_str, &r_garr);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Failed to setup json response");
        return rv;
    }
    json_object_set_string(r_vs, "mode", mode_str);

    groups = json_object_get_array(obj, "testGroups");
    g_cnt = json_array_get_count(groups);

    for (i = 0; i < g_cnt; i++) {
        int tgId = 0;
        groupval = json_array_get_value(groups, i);
        groupobj = json_value_get_object(groupval);

        /*
         * Create a new group in the response with the tgid
         * and an array of tests
         */
        r_gval = json_value_init_object();
        r_gobj = json_value_get_object(r_gval);
        tgId = json_object_get_number(groupobj, "tgId");
        if (!tgId) {
            AMVP_LOG_ERR("Missing tgid from server JSON groub obj");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
        json_object_set_number(r_gobj, "tgId", tgId);
        json_object_set_value(r_gobj, "tests", json_value_init_array());
        r_tarr = json_object_get_array(r_gobj, "tests");

        test_type_str = json_object_get_string(groupobj, "testType");
        if (!test_type_str) {
            AMVP_LOG_ERR("Missing testType from server JSON for RSA keyGen");
            rv = AMVP_TC_INVALID_DATA;
            goto err;
        }
        test_type = read_test_type(test_type_str);

        info_gen_by_server = json_object_get_boolean(groupobj, "infoGeneratedByServer");
        if (info_gen_by_server == -1) {
            AMVP_LOG_ERR("Server JSON missing 'infoGeneratedByServer'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }

        pub_exp_mode_str = json_object_get_string(groupobj, "pubExp");
        if (!pub_exp_mode_str) {
            AMVP_LOG_ERR("Server JSON missing 'pubExpMode'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }
        pub_exp_mode = read_pub_exp_mode(pub_exp_mode_str);
        if (!pub_exp_mode) {
            AMVP_LOG_ERR("Server JSON invalid 'pubExpMode'");
            rv = AMVP_INVALID_ARG;
            goto err;
        }

        if (pub_exp_mode == AMVP_RSA_PUB_EXP_MODE_FIXED) {
            e_str = json_object_get_string(groupobj, "fixedPubExp");
            if (!e_str) {
                AMVP_LOG_ERR("Server JSON missing 'fixedPubExp'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
        }

        key_format_str = json_object_get_string(groupobj, "keyFormat");
        if (!key_format_str) {
            AMVP_LOG_ERR("Server JSON missing 'keyFormat'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }
        key_format = read_key_format(key_format_str);
        if (!key_format) {
            AMVP_LOG_ERR("Server JSON invalid 'keyFormat'");
            rv = AMVP_INVALID_ARG;
            goto err;
        }

        rand_pq_str = json_object_get_string(groupobj, "randPQ");
        if (!rand_pq_str) {
            AMVP_LOG_ERR("Server JSON missing 'randPQ'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }
        rand_pq = amvp_lookup_rsa_randpq_index(rand_pq_str);
        if (rand_pq == 0) {
            AMVP_LOG_ERR("Server JSON invalid randPQ");
            rv = AMVP_INVALID_ARG;
            goto err;
        }

        if (rand_pq == AMVP_RSA_KEYGEN_B33 ||
            rand_pq == AMVP_RSA_KEYGEN_B35 ||
            rand_pq == AMVP_RSA_KEYGEN_B36) {
            prime_test_str = json_object_get_string(groupobj, "primeTest");
            if (!prime_test_str) {
                AMVP_LOG_ERR("Server JSON missing 'primeTest'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }

            prime_test = read_prime_test_type(prime_test_str);
            if (!prime_test) {
                AMVP_LOG_ERR("Server JSON invalid 'primeTest'");
                rv = AMVP_INVALID_ARG;
                goto err;
            }
        }

        mod = json_object_get_number(groupobj, "modulo");
        if (!mod) {
            AMVP_LOG_ERR("Server JSON missing 'modulo'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }
        if (mod != 2048 && mod != 3072 && mod != 4096) {
            AMVP_LOG_ERR("Server JSON invalid 'modulo', (%d)", mod);
            rv = AMVP_INVALID_ARG;
            goto err;
        }

        if (rand_pq == AMVP_RSA_KEYGEN_B32 ||
            rand_pq == AMVP_RSA_KEYGEN_B34 ||
            rand_pq == AMVP_RSA_KEYGEN_B35) {
            hash_alg_str = json_object_get_string(groupobj, "hashAlg");
            if (!hash_alg_str) {
                AMVP_LOG_ERR("Server JSON missing 'hashAlg'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            hash_alg = amvp_lookup_hash_alg(hash_alg_str);
            if (!hash_alg) {
                AMVP_LOG_ERR("Server JSON invalid 'hashAlg'");
                rv = AMVP_INVALID_ARG;
                goto err;
            }
        }

        AMVP_LOG_VERBOSE("    Test group: %d", i);
        AMVP_LOG_VERBOSE("  infoGenByServer: %s", info_gen_by_server ? "true" : "false");
        AMVP_LOG_VERBOSE("       pubExpMode: %s", pub_exp_mode_str);
        AMVP_LOG_VERBOSE("        keyFormat: %s", key_format_str);
        AMVP_LOG_VERBOSE("           modulo: %d", mod);

        tests = json_object_get_array(groupobj, "tests");
        t_cnt = json_array_get_count(tests);

        for (j = 0; j < t_cnt; j++) {
            AMVP_LOG_VERBOSE("Found new RSA test vector...");
            testval = json_array_get_value(tests, j);
            testobj = json_value_get_object(testval);
            tc_id = json_object_get_number(testobj, "tcId");

            AMVP_LOG_VERBOSE("        Test case: %d", j);
            AMVP_LOG_VERBOSE("             tcId: %d", tc_id);

            /*
             * Create a new test case in the response
             */
            r_tval = json_value_init_object();
            r_tobj = json_value_get_object(r_tval);

            json_object_set_number(r_tobj, "tcId", tc_id);

            if (pub_exp_mode == AMVP_RSA_PUB_EXP_MODE_RANDOM) {
                e_str = json_object_get_string(testobj, "e");
                if (!e_str) {
                    AMVP_LOG_ERR("Server JSON missing 'e'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
                if (strnlen_s(e_str, AMVP_RSA_EXP_LEN_MAX + 1)
                    > AMVP_RSA_EXP_LEN_MAX) {
                    AMVP_LOG_ERR("'e' too long, max allowed=(%d)",
                                    AMVP_RSA_EXP_LEN_MAX);
                    rv = AMVP_INVALID_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
            }
            /*
             * Retrieve values from JSON and initialize the tc
             */
            if (info_gen_by_server) {
                unsigned int count = 0;

                bitlens = json_object_get_array(testobj, "bitlens");
                count = json_array_get_count(bitlens);
                if (count != 4) {
                    AMVP_LOG_ERR("Server JSON 'bitlens' list count is (%u). Expected (%u)",
                                 count, 4);
                    rv = AMVP_INVALID_ARG;
                    json_value_free(r_tval);
                    goto err;
                }

                bitlen1 = json_array_get_number(bitlens, 0);
                bitlen2 = json_array_get_number(bitlens, 1);
                bitlen3 = json_array_get_number(bitlens, 2);
                bitlen4 = json_array_get_number(bitlens, 3);

                if (rand_pq == AMVP_RSA_KEYGEN_B32 ||
                    rand_pq == AMVP_RSA_KEYGEN_B34 ||
                    rand_pq == AMVP_RSA_KEYGEN_B35) {
                    seed = json_object_get_string(testobj, "seed");
                    if (!seed) {
                        AMVP_LOG_ERR("Server JSON missing 'seed'");
                        rv = AMVP_MISSING_ARG;
                        json_value_free(r_tval);
                        goto err;
                    }
                    seed_len = strnlen_s(seed, AMVP_RSA_SEEDLEN_MAX + 1);
                    if (seed_len > AMVP_RSA_SEEDLEN_MAX) {
                        AMVP_LOG_ERR("'seed' too long, max allowed=(%d)",
                                    AMVP_RSA_SEEDLEN_MAX);
                        rv = AMVP_INVALID_ARG;
                        json_value_free(r_tval);
                        goto err;
                    }
                }
            }

            /* for B.3.6, test cases also come with xP, xP1, xP2, xQ, xQ1, xQ2 */
            if (rand_pq == AMVP_RSA_KEYGEN_B36) {
                xp_str = json_object_get_string(testobj, "xP");
                if (!xp_str) {
                    AMVP_LOG_ERR("Server JSON missing 'xP'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
                xp1_str = json_object_get_string(testobj, "xP1");
                if (!xp1_str) {
                    AMVP_LOG_ERR("Server JSON missing 'xP1'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
                xp2_str = json_object_get_string(testobj, "xP2");
                if (!xp2_str) {
                    AMVP_LOG_ERR("Server JSON missing 'xP2'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
                xq_str = json_object_get_string(testobj, "xQ");
                if (!xq_str) {
                    AMVP_LOG_ERR("Server JSON missing 'xQ'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
                xq1_str = json_object_get_string(testobj, "xQ1");
                if (!xq1_str) {
                    AMVP_LOG_ERR("Server JSON missing 'xQ1'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
                xq2_str = json_object_get_string(testobj, "xQ2");
                if (!xq2_str) {
                    AMVP_LOG_ERR("Server JSON missing 'xQ2'");
                    rv = AMVP_MISSING_ARG;
                    json_value_free(r_tval);
                    goto err;
                }
            }

            rv = amvp_rsa_keygen_init_tc(ctx, &stc, tc_id, test_type, info_gen_by_server, hash_alg, 
                                         key_format, pub_exp_mode, mod, prime_test, rand_pq, e_str,
                                         p_str, q_str, xp_str, xp1_str, xp2_str, xq_str, xq1_str, 
                                         xq2_str, seed, seed_len, bitlen1, bitlen2, bitlen3, bitlen4);

            /* Process the current test vector... */
            if (rv == AMVP_SUCCESS) {
                if ((cap->crypto_handler)(&tc)) {
                    AMVP_LOG_ERR("ERROR: crypto module failed the operation");
                    rv = AMVP_CRYPTO_MODULE_FAIL;
                    json_value_free(r_tval);
                    goto err;
                }
            }

            /*
             * Output the test case results using JSON
             */
            rv = amvp_rsa_output_tc(ctx, &stc, r_tobj);
            if (rv != AMVP_SUCCESS) {
                AMVP_LOG_ERR("ERROR: JSON output failure in hash module");
                json_value_free(r_tval);
                goto err;
            }

            /*
             * Release all the memory associated with the test case
             */
            amvp_rsa_keygen_release_tc(&stc);

            /* Append the test response value to array */
            json_array_append_value(r_tarr, r_tval);
        }
        json_array_append_value(r_garr, r_gval);
    }

    json_array_append_value(reg_arry, r_vs_val);

    json_result = json_serialize_to_string_pretty(ctx->kat_resp, NULL);
    AMVP_LOG_VERBOSE("\n\n%s\n\n", json_result);
    json_free_serialized_string(json_result);
    rv = AMVP_SUCCESS;

err:
    if (rv != AMVP_SUCCESS) {
        amvp_rsa_keygen_release_tc(&stc);
        amvp_release_json(r_vs_val, r_gval);
    }
    return rv;
}
