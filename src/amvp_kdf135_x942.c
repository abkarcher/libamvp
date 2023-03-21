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
static AMVP_RESULT amvp_kdf135_x942_output_tc(AMVP_CTX *ctx, AMVP_KDF135_X942_TC *stc, JSON_Object *tc_rsp) {
    AMVP_RESULT rv;
    char *tmp = NULL;

    if (stc->dkm_len == stc->key_len) {
        tmp = calloc(AMVP_KDF135_X942_STR_MAX + 1, sizeof(char));
        if (!tmp) {
            AMVP_LOG_ERR("Error allocating memory in X942 KDF TC output");
            return AMVP_MALLOC_FAIL;
        }
        rv = amvp_bin_to_hexstr(stc->dkm, stc->dkm_len, tmp, AMVP_KDF135_X942_STR_MAX);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (dkm)");
            goto err;
        }
        json_object_set_string(tc_rsp, "derivedKey", (const char *)tmp);
    } else {
        AMVP_LOG_ERR("Error outputting test case for X942 KDF. Dkm_len MUST equal key_len.");
        rv = AMVP_TC_INVALID_DATA;
        goto err;
    }
    rv = AMVP_SUCCESS;
err:
    if (tmp) free(tmp);
    return rv;
}

/*
 * This function simply releases the data associated with
 * a test case.
 */
static AMVP_RESULT amvp_kdf135_x942_release_tc(AMVP_KDF135_X942_TC *stc) {
    if (stc->oid) free(stc->oid);
    if (stc->zz) free(stc->zz);
    if (stc->party_u_info) free(stc->party_u_info);
    if (stc->party_v_info) free(stc->party_v_info);
    if (stc->supp_pub_info) free(stc->supp_pub_info);
    if (stc->supp_priv_info) free(stc->supp_priv_info);
    if (stc->dkm) free(stc->dkm);
    memzero_s(stc, sizeof(AMVP_KDF135_X942_TC));
    return AMVP_SUCCESS;
}

static AMVP_RESULT amvp_kdf135_x942_init_tc(AMVP_CTX *ctx,
                                            AMVP_KDF135_X942_TC *stc,
                                            int tc_id,
                                            AMVP_HASH_ALG hash_alg,
                                            AMVP_KDF_X942_TYPE type,
                                            const char *oid,
                                            const char *zz,
                                            int key_len,
                                            const char *party_u_info,
                                            const char *party_v_info,
                                            const char *supp_pub_info,
                                            const char *supp_priv_info) {
    AMVP_RESULT rv = AMVP_SUCCESS;

    memzero_s(stc, sizeof(AMVP_KDF135_X942_TC));

    stc->tc_id = tc_id;
    stc->hash_alg = hash_alg;
    stc->key_len = key_len / 8;
    stc->type = type;

    stc->dkm = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->dkm) { return AMVP_MALLOC_FAIL; }

    stc->oid = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->oid) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(oid, stc->oid, AMVP_KDF135_X942_BYTE_MAX, &stc->oid_len);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (oid)");
        return rv;
    }

    stc->zz = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->zz) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(zz, stc->zz, AMVP_KDF135_X942_BYTE_MAX, &stc->zz_len);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (zz)");
        return rv;
    }

    stc->party_u_info = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->party_u_info) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(party_u_info, stc->party_u_info, AMVP_KDF135_X942_BYTE_MAX, &stc->party_u_len);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (party_u_info)");
        return rv;
    }

    stc->party_v_info = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->party_v_info) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(party_v_info, stc->party_v_info, AMVP_KDF135_X942_BYTE_MAX, &stc->party_v_len);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (party_v_info)");
        return rv;
    }

    stc->supp_pub_info = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->supp_pub_info) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(supp_pub_info, stc->supp_pub_info, AMVP_KDF135_X942_BYTE_MAX, &stc->supp_pub_len);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (sup_pub_info)");
        return rv;
    }

    stc->supp_priv_info = calloc(AMVP_KDF135_X942_BYTE_MAX, sizeof(unsigned char));
    if (!stc->supp_priv_info) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(supp_priv_info, stc->supp_priv_info, AMVP_KDF135_X942_BYTE_MAX, &stc->supp_priv_len);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (supp_priv_info)");
        return rv;
    }

    return AMVP_SUCCESS;
}

AMVP_RESULT amvp_kdf135_x942_kat_handler(AMVP_CTX *ctx, JSON_Object *obj) {
    JSON_Value *groupval;
    JSON_Object *groupobj = NULL;
    JSON_Value *testval;
    JSON_Object *testobj = NULL;
    JSON_Array *groups;
    JSON_Array *tests;

    JSON_Value *reg_arry_val = NULL;
    JSON_Object *reg_obj = NULL;
    JSON_Array *reg_arry = NULL;

    JSON_Value *r_vs_val = NULL;
    JSON_Object *r_vs = NULL;
    JSON_Array *r_tarr = NULL, *r_garr = NULL;  /* Response testarray, grouparray */
    JSON_Value *r_tval = NULL, *r_gval = NULL;  /* Response testval, groupval */
    JSON_Object *r_tobj = NULL, *r_gobj = NULL; /* Response testobj, groupobj */

    AMVP_CAPS_LIST *cap;
    AMVP_KDF135_X942_TC stc;
    AMVP_TEST_CASE tc;
    AMVP_RESULT rv;
    int tc_id = 0, i = 0, g_cnt = 0, j = 0, t_cnt = 0, diff = 0, len = 0, key_len = 0;
    const char *alg_str = NULL, *mode_str = NULL, *kdf_type_str = NULL, *oid = NULL, *party_u = NULL,
               *party_v = NULL, *supp_pub = NULL, *supp_priv = NULL, *zz = NULL;
    AMVP_CIPHER alg_id;
    AMVP_KDF_X942_TYPE kdf_type;
    char *json_result;

    if (!ctx) {
        AMVP_LOG_ERR("No ctx for handler operation");
        return AMVP_NO_CTX;
    }

    if (!obj) {
        AMVP_LOG_ERR("No obj for handler operation");
        return AMVP_MALFORMED_JSON;
    }

    alg_str = json_object_get_string(obj, "algorithm");
    if (!alg_str) {
        AMVP_LOG_ERR("Server JSON missing 'algorithm'");
        return AMVP_MISSING_ARG;
    }

    mode_str = json_object_get_string(obj, "mode");
    if (!mode_str) {
        AMVP_LOG_ERR("Server JSON missing 'mode'");
        return AMVP_MISSING_ARG;
    }
    
    alg_id = amvp_lookup_cipher_w_mode_index(alg_str, mode_str);
    if (alg_id != AMVP_KDF135_X942) {
        AMVP_LOG_ERR("Server JSON invalid 'algorithm' or 'mode'");
        return AMVP_INVALID_ARG;
    }

    /*
     * Get a reference to the abstracted test case
     */
    tc.tc.kdf135_x942 = &stc;
    stc.cipher = alg_id;

    cap = amvp_locate_cap_entry(ctx, alg_id);
    if (!cap) {
        AMVP_LOG_ERR("AMVP server requesting unsupported capability %s : %d.", alg_str, alg_id);
        return AMVP_UNSUPPORTED_OP;
    }

    /*
     * Create AMVP array for response
     */
    rv = amvp_create_array(&reg_obj, &reg_arry_val, &reg_arry);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Failed to create JSON response struct. ");
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
    json_object_set_string(r_vs, "mode", "ansix9.42");

    groups = json_object_get_array(obj, "testGroups");
    if (!groups) {
        AMVP_LOG_ERR("Failed to include testGroups. ");
        rv = AMVP_MISSING_ARG;
        goto err;
    }

    g_cnt = json_array_get_count(groups);
    for (i = 0; i < g_cnt; i++) {
        int tgId = 0;
        AMVP_HASH_ALG hash_alg = 0;
        const char *hash_alg_str = NULL;

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

        kdf_type_str = json_object_get_string(groupobj, "kdfType");
        if (!kdf_type_str) {
            AMVP_LOG_ERR("Server JSON missing 'kdfType'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }
        strncmp_s(kdf_type_str, 14, "DER", 3, &diff);
        if (!diff) {
            kdf_type = AMVP_KDF_X942_KDF_TYPE_DER;
       } else {
            strncmp_s(kdf_type_str, 14, "concatentation", 14, &diff);
            if (!diff) {
                kdf_type = AMVP_KDF_X942_KDF_TYPE_CONCAT;
            } else {
                AMVP_LOG_ERR("Server JSON invalid 'kdfType'");
                rv = AMVP_INVALID_ARG;
                goto err;
            }
       }

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
    
        oid = json_object_get_string(groupobj, "oid");
        if (!oid) {
            AMVP_LOG_ERR("Server JSON missing 'oid'");
            rv = AMVP_MISSING_ARG;
            goto err;
        }

        AMVP_LOG_VERBOSE("\n    Test group: %d", i);
        AMVP_LOG_VERBOSE("         kdfType: %s", kdf_type_str);
        AMVP_LOG_VERBOSE("         hashAlg: %s", hash_alg_str);
        AMVP_LOG_VERBOSE("             OID: %s", oid);

        tests = json_object_get_array(groupobj, "tests");
        if (!tests) {
            AMVP_LOG_ERR("Failed to include tests. ");
            rv = AMVP_MISSING_ARG;
            goto err;
        }

        t_cnt = json_array_get_count(tests);
        if (!t_cnt) {
            AMVP_LOG_ERR("Failed to include tests in array. ");
            rv = AMVP_MISSING_ARG;
            goto err;
        }

        for (j = 0; j < t_cnt; j++) {
            AMVP_LOG_VERBOSE("Found new KDF135 X942 test vector...");
            testval = json_array_get_value(tests, j);
            testobj = json_value_get_object(testval);

            tc_id = json_object_get_number(testobj, "tcId");
            if (!tc_id) {
                AMVP_LOG_ERR("Server JSON missing 'tcId'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }

            zz = json_object_get_string(testobj, "zz");
            if (!zz) {
                AMVP_LOG_ERR("Server JSON missing 'zz'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            len = strnlen_s(zz, AMVP_KDF135_X942_STR_MAX + 1);
            if (len > AMVP_KDF135_X942_STR_MAX) {
                AMVP_LOG_ERR("Server JSON invalid 'zz' (max = %d, given = %d)", AMVP_KDF135_X942_STR_MAX, len);
                rv = AMVP_INVALID_ARG;
                goto err;
            }

            key_len = json_object_get_number(testobj, "keyLen");
            if (!key_len) {
                AMVP_LOG_ERR("Server JSON missing 'keyLen'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            if (key_len < 1 || key_len > AMVP_KDF135_X942_BIT_MAX) {
                AMVP_LOG_ERR("Server JSON invalid 'keyLen' (given: %d)", key_len);
                rv = AMVP_INVALID_ARG;
                goto err;
            }

            party_u = json_object_get_string(testobj, "partyUInfo");
            if (!party_u) {
                AMVP_LOG_ERR("Server JSON missing 'partyUInfo'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            len = strnlen_s(party_u, AMVP_KDF135_X942_STR_MAX + 1);
            if (len > AMVP_KDF135_X942_STR_MAX) {
                AMVP_LOG_ERR("Server JSON invalid 'partyUInfo' (max = %d, given = %d)", AMVP_KDF135_X942_STR_MAX, len);
                rv = AMVP_INVALID_ARG;
                goto err;
            }

            party_v = json_object_get_string(testobj, "partyVInfo");
            if (!party_v) {
                AMVP_LOG_ERR("Server JSON missing 'partyVInfo'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            len = strnlen_s(party_v, AMVP_KDF135_X942_STR_MAX + 1);
            if (len > AMVP_KDF135_X942_STR_MAX) {
                AMVP_LOG_ERR("Server JSON invalid 'partyVInfo' (max = %d, given = %d)", AMVP_KDF135_X942_STR_MAX, len);
                rv = AMVP_INVALID_ARG;
                goto err;
            }

            supp_pub = json_object_get_string(testobj, "suppPubInfo");
            if (!supp_pub) {
                AMVP_LOG_ERR("Server JSON missing 'suppPubInfo'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            len = strnlen_s(supp_pub, AMVP_KDF135_X942_STR_MAX + 1);
            if (len > AMVP_KDF135_X942_STR_MAX) {
                AMVP_LOG_ERR("Server JSON invalid 'suppPubInfo' (max = %d, given = %d)", AMVP_KDF135_X942_STR_MAX, len);
                rv = AMVP_INVALID_ARG;
                goto err;
            }

            supp_priv = json_object_get_string(testobj, "suppPrivInfo");
            if (!supp_priv) {
                AMVP_LOG_ERR("Server JSON missing 'suppPrivInfo'");
                rv = AMVP_MISSING_ARG;
                goto err;
            }
            len = strnlen_s(supp_priv, AMVP_KDF135_X942_STR_MAX + 1);
            if (len > AMVP_KDF135_X942_STR_MAX) {
                AMVP_LOG_ERR("Server JSON invalid 'suppPrivInfo' (max = %d, given = %d)", AMVP_KDF135_X942_STR_MAX, len);
                rv = AMVP_INVALID_ARG;
                goto err;
            }

            AMVP_LOG_VERBOSE("        Test case: %d", j);
            AMVP_LOG_VERBOSE("             tcId: %d", tc_id);
            AMVP_LOG_VERBOSE("               zz: %s", zz);
            AMVP_LOG_VERBOSE("           keyLen: %d", key_len);
            AMVP_LOG_VERBOSE("       partyUInfo: %s", party_u);
            AMVP_LOG_VERBOSE("       partyVInfo: %s", party_v);
            AMVP_LOG_VERBOSE("      suppPubInfo: %s", supp_pub);
            AMVP_LOG_VERBOSE("     suppPrivInfo: %s", supp_priv);

            /*
             * Create a new test case in the response
             */
            r_tval = json_value_init_object();
            r_tobj = json_value_get_object(r_tval);

            json_object_set_number(r_tobj, "tcId", tc_id);

            /*
             * Setup the test case data that will be passed down to
             * the crypto module.
             */
            rv = amvp_kdf135_x942_init_tc(ctx, &stc, tc_id, hash_alg,
                                          kdf_type, oid, zz, key_len,
                                          party_u, party_v, supp_pub, supp_priv);
            if (rv != AMVP_SUCCESS) {
                amvp_kdf135_x942_release_tc(&stc);
                json_value_free(r_tval);
                goto err;
            }

            /* Process the current test vector... */
            if ((cap->crypto_handler)(&tc)) {
                AMVP_LOG_ERR("crypto module failed the KDF X942 operation");
                amvp_kdf135_x942_release_tc(&stc);
                rv = AMVP_CRYPTO_MODULE_FAIL;
                json_value_free(r_tval);
                goto err;
            }

            /*
             * Output the test case results using JSON
             */
            rv = amvp_kdf135_x942_output_tc(ctx, &stc, r_tobj);
            if (rv != AMVP_SUCCESS) {
                AMVP_LOG_ERR("JSON output failure in hash module");
                amvp_kdf135_x942_release_tc(&stc);
                json_value_free(r_tval);
                goto err;
            }
            /*
             * Release all the memory associated with the test case
             */
            amvp_kdf135_x942_release_tc(&stc);

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
        amvp_release_json(r_vs_val, r_gval);
    }
    return rv;
}
