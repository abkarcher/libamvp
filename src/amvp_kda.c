/** @file */
/*
 * Copyright (c) 2021, Cisco Systems, Inc.
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
static AMVP_RESULT amvp_kda_hkdf_output_tc(AMVP_CTX *ctx,
                                               AMVP_KDA_HKDF_TC *stc,
                                               JSON_Object *tc_rsp) {
    AMVP_RESULT rv = AMVP_SUCCESS;
    char *tmp = NULL;

    tmp = calloc(AMVP_KDA_DKM_STR_MAX + 1, sizeof(char));
    if (!tmp) {
        AMVP_LOG_ERR("Unable to malloc in amvp_kda_hkdf_output_tc");
        return AMVP_MALLOC_FAIL;
    }

    if (stc->type == AMVP_KDA_TT_VAL) {
        int diff = 1;

        memcmp_s(stc->outputDkm, AMVP_KDA_DKM_BYTE_MAX,
                 stc->providedDkm, AMVP_KDA_DKM_BYTE_MAX, &diff);
        if (!diff) {
            json_object_set_boolean(tc_rsp, "testPassed", 1);
        } else {
            json_object_set_boolean(tc_rsp, "testPassed", 0);
        }
        goto end;
    }

    memzero_s(tmp, AMVP_KDA_DKM_STR_MAX);
    rv = amvp_bin_to_hexstr(stc->outputDkm, stc->l, tmp, AMVP_KDA_DKM_STR_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (dkm)");
        goto end;
    }
    json_object_set_string(tc_rsp, "dkm", tmp);


end:
    if (tmp) free(tmp);

    return rv;
}

static AMVP_RESULT amvp_kda_onestep_output_tc(AMVP_CTX *ctx,
                                               AMVP_KDA_ONESTEP_TC *stc,
                                               JSON_Object *tc_rsp) {
    AMVP_RESULT rv = AMVP_SUCCESS;
    char *tmp = NULL;

    tmp = calloc(AMVP_KDA_DKM_STR_MAX + 1, sizeof(char));
    if (!tmp) {
        AMVP_LOG_ERR("Unable to malloc in amvp_kda_onestep_output_tc");
        return AMVP_MALLOC_FAIL;
    }

    if (stc->type == AMVP_KDA_TT_VAL) {
        int diff = 1;

        memcmp_s(stc->outputDkm, AMVP_KDA_DKM_BYTE_MAX,
                 stc->providedDkm, AMVP_KDA_DKM_BYTE_MAX, &diff);
        if (!diff) {
            json_object_set_boolean(tc_rsp, "testPassed", 1);
        } else {
            json_object_set_boolean(tc_rsp, "testPassed", 0);
        }
        goto end;
    }

    memzero_s(tmp, AMVP_KDA_DKM_STR_MAX);
    rv = amvp_bin_to_hexstr(stc->outputDkm, stc->l, tmp, AMVP_KDA_DKM_STR_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (dkm)");
        goto end;
    }
    json_object_set_string(tc_rsp, "dkm", tmp);


end:
    if (tmp) free(tmp);

    return rv;
}

static AMVP_RESULT amvp_kda_onestep_init_tc(AMVP_CTX *ctx,
                                             AMVP_KDA_ONESTEP_TC *stc,
                                             const int tc_id,
                                             AMVP_CIPHER aux_function,
                                             const char *salt,
                                             const char *z,
                                             const char *t,
                                             const char *uparty,
                                             const char *uephemeral,
                                             const char *vparty,
                                             const char *vephemeral,
                                             const char *algid,
                                             const char *context,
                                             const char *label,
                                             const char *providedDkm,
                                             const int l,
                                             const int saltLen,
                                             AMVP_KDA_MAC_SALT_METHOD saltMethod,
                                             AMVP_KDA_ENCODING encoding,
                                             AMVP_KDA_PATTERN_CANDIDATE *fixedArr,
                                             AMVP_KDA_TEST_TYPE test_type) {
    AMVP_RESULT rv;

    stc->tc_id = tc_id;
    stc->type = test_type;
    stc->aux_function = aux_function;
    stc->l = l / 8;
    stc->encoding = encoding;
    stc->saltMethod = saltMethod;

    if (memcpy_s(stc->fixedInfoPattern, AMVP_KDA_PATTERN_MAX * sizeof(int), fixedArr, AMVP_KDA_PATTERN_MAX * sizeof(int))) {
        AMVP_LOG_ERR("Error copying array of fixedInfoPattern candidates into test case structure");
        rv = AMVP_MALLOC_FAIL;
        return rv;
    } 
    if (salt) {
        stc->salt = calloc(1, AMVP_KDA_SALT_BYTE_MAX);
        if (!stc->salt) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(salt, stc->salt, AMVP_KDA_SALT_BYTE_MAX, &(stc->saltLen));
        if (rv != AMVP_SUCCESS || (saltLen != 0 && stc->saltLen != saltLen / 8)) {
            AMVP_LOG_ERR("Hex conversion failure (salt)");
            return rv;
        }
    }

    stc->z = calloc(1, AMVP_KDA_Z_BYTE_MAX);
    if (!stc->z) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(z, stc->z, AMVP_KDA_Z_BYTE_MAX, &(stc->zLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (z)");
        return rv;
    }

    if (t) {
        stc->t = calloc(1, AMVP_KDA_Z_BYTE_MAX);
        if (!stc->t) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(t, stc->t, AMVP_KDA_Z_BYTE_MAX, &(stc->tLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (t)");
            return rv;
        }
    }

    stc->uPartyId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
    if (!stc->uPartyId) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(uparty, stc->uPartyId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->uPartyIdLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (uPartyId)");
        return rv;
    }
    if (uephemeral) {
        stc->uEphemeralData = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->uEphemeralData) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(uephemeral, stc->uEphemeralData, AMVP_KDA_FIXED_BYTE_MAX, &(stc->uEphemeralLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (uEphemeral)");
            return rv;
        }
    }

    stc->vPartyId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
    if (!stc->vPartyId) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(vparty, stc->vPartyId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->vPartyIdLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (vPartyid)");
        return rv;
    }
    if (vephemeral) {
        stc->vEphemeralData = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->vEphemeralData) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(vephemeral, stc->vEphemeralData, AMVP_KDA_FIXED_BYTE_MAX, &(stc->vEphemeralLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (vEphemeral)");
            return rv;
        }
    }

    if (algid) {
        stc->algorithmId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->algorithmId) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(algid, stc->algorithmId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->algIdLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (algorithmId)");
            return rv;
        }
    }

    if (label) {
        stc->label = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->label) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(label, stc->label, AMVP_KDA_FIXED_BYTE_MAX, &(stc->labelLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (label)");
            return rv;
        }
    }

    if (context) {
        stc->context = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->context) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(context, stc->context, AMVP_KDA_FIXED_BYTE_MAX, &(stc->contextLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (algorithmId)");
            return rv;
        }
    }

    stc->outputDkm = calloc(AMVP_KDA_DKM_BYTE_MAX, 1);
    if (!stc->outputDkm) { 
        AMVP_LOG_ERR("Failed to allocate outputDkm initializing test case");
        return AMVP_MALLOC_FAIL; 
    }

    if (stc->type == AMVP_KDA_TT_VAL) {
        stc->providedDkm = calloc(AMVP_KDA_DKM_BYTE_MAX, 1);
        if (!stc->providedDkm) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(providedDkm, stc->providedDkm, AMVP_KDA_DKM_BYTE_MAX, NULL);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (dkm)");
            return rv;
        }
    }

    return AMVP_SUCCESS;
}

static AMVP_RESULT amvp_kda_twostep_output_tc(AMVP_CTX *ctx,
                                               AMVP_KDA_TWOSTEP_TC *stc,
                                               JSON_Object *tc_rsp) {
    AMVP_RESULT rv = AMVP_SUCCESS;
    char *tmp = NULL;

    tmp = calloc(AMVP_KDA_DKM_STR_MAX + 1, sizeof(char));
    if (!tmp) {
        AMVP_LOG_ERR("Unable to malloc in amvp_kda_twostep_output_tc");
        return AMVP_MALLOC_FAIL;
    }

    if (stc->type == AMVP_KDA_TT_VAL) {
        int diff = 1;

        memcmp_s(stc->outputDkm, AMVP_KDA_DKM_BYTE_MAX,
                 stc->providedDkm, AMVP_KDA_DKM_BYTE_MAX, &diff);

        if (!diff) {
            json_object_set_boolean(tc_rsp, "testPassed", 1);
        } else {
            json_object_set_boolean(tc_rsp, "testPassed", 0);
        }
        goto end;
    }

    memzero_s(tmp, AMVP_KDA_DKM_STR_MAX);
    rv = amvp_bin_to_hexstr(stc->outputDkm, stc->l, tmp, AMVP_KDA_DKM_STR_MAX);
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("hex conversion failure (dkm)");
        goto end;
    }
    json_object_set_string(tc_rsp, "dkm", tmp);


end:
    if (tmp) free(tmp);

    return rv;
}

static AMVP_RESULT amvp_kda_twostep_init_tc(AMVP_CTX *ctx,
                                             AMVP_KDA_TWOSTEP_TC *stc,
                                             const int tc_id,
                                             AMVP_KDF108_MAC_MODE_VAL mac_mode,
                                             int hybrid_secret,
                                             const char *salt,
                                             const char *z,
                                             const char *iv,
                                             const char *t,
                                             const char *uparty,
                                             const char *uephemeral,
                                             const char *vparty,
                                             const char *vephemeral,
                                             const char *algid,
                                             const char *context,
                                             const char *label,
                                             const char *providedDkm,
                                             const int l,
                                             const int saltLen,
                                             const int ivLen,
                                             const int counterLen,
                                             AMVP_KDA_MAC_SALT_METHOD saltMethod,
                                             AMVP_KDF108_MODE kdfMode,
                                             AMVP_KDF108_FIXED_DATA_ORDER_VAL counterLocation,
                                             AMVP_KDA_ENCODING encoding,
                                             AMVP_KDA_PATTERN_CANDIDATE *fixedArr,
                                             AMVP_KDA_TEST_TYPE test_type) {
    AMVP_RESULT rv;

    stc->tc_id = tc_id;
    stc->type = test_type;
    stc->macFunction = mac_mode;
    stc->l = l / 8;
    stc->encoding = encoding;
    stc->saltMethod = saltMethod;
    stc->kdfMode = kdfMode;
    stc->counterLocation = counterLocation;
    stc->counterLen = counterLen;
    stc->uses_hybrid_secret = hybrid_secret;

    if (memcpy_s(stc->fixedInfoPattern, AMVP_KDA_PATTERN_MAX * sizeof(int), fixedArr, AMVP_KDA_PATTERN_MAX * sizeof(int))) {
        AMVP_LOG_ERR("Error copying array of fixedInfoPattern candidates into test case structure");
        rv = AMVP_MALLOC_FAIL;
        return rv;
    } 

    stc->salt = calloc(1, AMVP_KDA_SALT_BYTE_MAX);
    if (!stc->salt) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(salt, stc->salt, AMVP_KDA_SALT_BYTE_MAX, &(stc->saltLen));
    if (rv != AMVP_SUCCESS || (saltLen != 0 && stc->saltLen != saltLen / 8)) {
        AMVP_LOG_ERR("Hex conversion failure (salt)");
        return rv;
    }


    stc->z = calloc(1, AMVP_KDA_Z_BYTE_MAX);
    if (!stc->z) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(z, stc->z, AMVP_KDA_Z_BYTE_MAX, &(stc->zLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (z)");
        return rv;
    }

    if (iv) {
        stc->iv = calloc(1, AMVP_KDA_Z_BYTE_MAX);
        if (!stc->iv) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(iv, stc->iv, AMVP_KDA_Z_BYTE_MAX, &(stc->ivLen));
        if (rv != AMVP_SUCCESS || (ivLen != 0 && stc->ivLen != ivLen / 8)) {
            AMVP_LOG_ERR("Hex conversion failure (iv)");
            return rv;
        }
    }

    if (t) {
        stc->t = calloc(1, AMVP_KDA_Z_BYTE_MAX);
        if (!stc->t) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(t, stc->t, AMVP_KDA_Z_BYTE_MAX, &(stc->tLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (t)");
            return rv;
        }
    }

    stc->uPartyId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
    if (!stc->uPartyId) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(uparty, stc->uPartyId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->uPartyIdLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (uPartyId)");
        return rv;
    }
    if (uephemeral) {
        stc->uEphemeralData = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->uEphemeralData) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(uephemeral, stc->uEphemeralData, AMVP_KDA_FIXED_BYTE_MAX, &(stc->uEphemeralLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (uEphemeral)");
            return rv;
        }
    }

    stc->vPartyId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
    if (!stc->vPartyId) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(vparty, stc->vPartyId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->vPartyIdLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (vPartyid)");
        return rv;
    }
    if (vephemeral) {
        stc->vEphemeralData = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->vEphemeralData) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(vephemeral, stc->vEphemeralData, AMVP_KDA_FIXED_BYTE_MAX, &(stc->vEphemeralLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (vEphemeral)");
            return rv;
        }
    }

    if (algid) {
        stc->algorithmId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->algorithmId) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(algid, stc->algorithmId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->algIdLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (algorithmId)");
            return rv;
        }
    }

    if (label) {
        stc->label = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->label) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(label, stc->label, AMVP_KDA_FIXED_BYTE_MAX, &(stc->labelLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (label)");
            return rv;
        }
    }

    if (context) {
        stc->context = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->context) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(context, stc->context, AMVP_KDA_FIXED_BYTE_MAX, &(stc->contextLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (algorithmId)");
            return rv;
        }
    }

    stc->outputDkm = calloc(AMVP_KDA_DKM_BYTE_MAX, 1);
    if (!stc->outputDkm) { 
        AMVP_LOG_ERR("Failed to allocate outputDkm initializing test case");
        return AMVP_MALLOC_FAIL; 
    }

    if (stc->type == AMVP_KDA_TT_VAL) {
        stc->providedDkm = calloc(AMVP_KDA_DKM_BYTE_MAX, 1);
        if (!stc->providedDkm) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(providedDkm, stc->providedDkm, AMVP_KDA_DKM_BYTE_MAX, NULL);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (dkm)");
            return rv;
        }
    }

    return AMVP_SUCCESS;
}

static AMVP_RESULT amvp_kda_hkdf_init_tc(AMVP_CTX *ctx,
                                             AMVP_KDA_HKDF_TC *stc,
                                             const int tc_id,
                                             AMVP_HASH_ALG hmac_alg,
                                             int hybrid_secret,
                                             const char *salt,
                                             const char *z,
                                             const char *t,
                                             const char *uparty,
                                             const char *uephemeral,
                                             const char *vparty,
                                             const char *vephemeral,
                                             const char *algid,
                                             const char *context,
                                             const char *label,
                                             const char *providedDkm,
                                             const int l,
                                             const int saltLen,
                                             AMVP_KDA_MAC_SALT_METHOD saltMethod,
                                             AMVP_KDA_ENCODING encoding,
                                             AMVP_KDA_PATTERN_CANDIDATE *fixedArr,
                                             AMVP_KDA_TEST_TYPE test_type) {
    AMVP_RESULT rv;

    stc->tc_id = tc_id;
    stc->type = test_type;
    stc->hmacAlg = hmac_alg;
    stc->l = l / 8;
    stc->encoding = encoding;
    stc->saltMethod = saltMethod;
    stc->uses_hybrid_secret = hybrid_secret;

    if (memcpy_s(stc->fixedInfoPattern, AMVP_KDA_PATTERN_MAX * sizeof(int), fixedArr, AMVP_KDA_PATTERN_MAX * sizeof(int))) {
        AMVP_LOG_ERR("Error copying array of fixedInfoPattern candidates into test case structure");
        rv = AMVP_MALLOC_FAIL;
        return rv;
    } 

    stc->salt = calloc(1, AMVP_KDA_SALT_BYTE_MAX);
    if (!stc->salt) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(salt, stc->salt, AMVP_KDA_SALT_BYTE_MAX, &(stc->saltLen));
    if (rv != AMVP_SUCCESS || stc->saltLen != saltLen / 8) {
        AMVP_LOG_ERR("Hex conversion failure (salt)");
        return rv;
    }

    stc->z = calloc(1, AMVP_KDA_Z_BYTE_MAX);
    if (!stc->z) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(z, stc->z, AMVP_KDA_Z_BYTE_MAX, &(stc->zLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (z)");
        return rv;
    }

    if (t) {
        stc->t = calloc(1, AMVP_KDA_Z_BYTE_MAX);
        if (!stc->t) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(t, stc->t, AMVP_KDA_Z_BYTE_MAX, &(stc->tLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (t)");
            return rv;
        }
    }

    stc->uPartyId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
    if (!stc->uPartyId) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(uparty, stc->uPartyId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->uPartyIdLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (uPartyId)");
        return rv;
    }
    if (uephemeral) {
        stc->uEphemeralData = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->uEphemeralData) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(uephemeral, stc->uEphemeralData, AMVP_KDA_FIXED_BYTE_MAX, &(stc->uEphemeralLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (uEphemeral)");
            return rv;
        }
    }

    stc->vPartyId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
    if (!stc->vPartyId) { return AMVP_MALLOC_FAIL; }
    rv = amvp_hexstr_to_bin(vparty, stc->vPartyId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->vPartyIdLen));
    if (rv != AMVP_SUCCESS) {
        AMVP_LOG_ERR("Hex conversion failure (vPartyid)");
        return rv;
    }
    if (vephemeral) {
        stc->vEphemeralData = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->vEphemeralData) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(vephemeral, stc->vEphemeralData, AMVP_KDA_FIXED_BYTE_MAX, &(stc->vEphemeralLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (vEphemeral)");
            return rv;
        }
    }

    if (algid) {
        stc->algorithmId = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->algorithmId) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(algid, stc->algorithmId, AMVP_KDA_FIXED_BYTE_MAX, &(stc->algIdLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (algorithmId)");
            return rv;
        }
    }

    if (label) {
        stc->label = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->label) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(label, stc->label, AMVP_KDA_FIXED_BYTE_MAX, &(stc->labelLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (label)");
            return rv;
        }
    }

    if (context) {
        stc->context = calloc(1, AMVP_KDA_FIXED_BYTE_MAX);
        if (!stc->context) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(context, stc->context, AMVP_KDA_FIXED_BYTE_MAX, &(stc->contextLen));
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (algorithmId)");
            return rv;
        }
    }

    stc->outputDkm = calloc(AMVP_KDA_DKM_BYTE_MAX, 1);
    if (!stc->outputDkm) { 
        AMVP_LOG_ERR("Failed to allocate outputDkm initializing test case");
        return AMVP_MALLOC_FAIL; 
    }

    if (stc->type == AMVP_KDA_TT_VAL) {
        stc->providedDkm = calloc(AMVP_KDA_DKM_BYTE_MAX, 1);
        if (!stc->providedDkm) { return AMVP_MALLOC_FAIL; }
        rv = amvp_hexstr_to_bin(providedDkm, stc->providedDkm, AMVP_KDA_DKM_BYTE_MAX, NULL);
        if (rv != AMVP_SUCCESS) {
            AMVP_LOG_ERR("Hex conversion failure (dkm)");
            return rv;
        }
    }

    return AMVP_SUCCESS;
}

/*
 * This function simply releases the data associated with
 * a test case.
 */
static AMVP_RESULT amvp_kda_release_tc(AMVP_CIPHER cipher, AMVP_TEST_CASE *tc) {
    if (cipher == AMVP_KDA_HKDF) {
        AMVP_KDA_HKDF_TC *stc = tc->tc.kda_hkdf;
        if (stc->salt) free(stc->salt);
        if (stc->z) free(stc->z);
        if (stc->t) free(stc->t);
        if (stc->literalCandidate) free(stc->literalCandidate);
        if (stc->algorithmId) free(stc->algorithmId);
        if (stc->label) free(stc->label);
        if (stc->context) free(stc->context);
        if (stc->uPartyId) free(stc->uPartyId);
        if (stc->uEphemeralData) free(stc->uEphemeralData);
        if (stc->vPartyId) free(stc->vPartyId);
        if (stc->vEphemeralData) free(stc->vEphemeralData);
        if (stc->providedDkm) free(stc->providedDkm);
        if (stc->outputDkm) free(stc->outputDkm);
        memzero_s(stc, sizeof(AMVP_KDA_HKDF_TC));
    } else if (cipher == AMVP_KDA_ONESTEP) {
        AMVP_KDA_ONESTEP_TC *stc = tc->tc.kda_onestep;
        if (stc->salt) free(stc->salt);
        if (stc->z) free(stc->z);
        if (stc->t) free(stc->t);
        if (stc->literalCandidate) free(stc->literalCandidate);
        if (stc->algorithmId) free(stc->algorithmId);
        if (stc->label) free(stc->label);
        if (stc->context) free(stc->context);
        if (stc->uPartyId) free(stc->uPartyId);
        if (stc->uEphemeralData) free(stc->uEphemeralData);
        if (stc->vPartyId) free(stc->vPartyId);
        if (stc->vEphemeralData) free(stc->vEphemeralData);
        if (stc->providedDkm) free(stc->providedDkm);
        if (stc->outputDkm) free(stc->outputDkm);
        memzero_s(stc, sizeof(AMVP_KDA_ONESTEP_TC));
    } else if (cipher == AMVP_KDA_TWOSTEP) {
        AMVP_KDA_TWOSTEP_TC *stc = tc->tc.kda_twostep;
        if (stc->salt) free(stc->salt);
        if (stc->iv) free (stc->iv);
        if (stc->z) free(stc->z);
        if (stc->t) free(stc->t);
        if (stc->literalCandidate) free(stc->literalCandidate);
        if (stc->algorithmId) free(stc->algorithmId);
        if (stc->label) free(stc->label);
        if (stc->context) free(stc->context);
        if (stc->uPartyId) free(stc->uPartyId);
        if (stc->uEphemeralData) free(stc->uEphemeralData);
        if (stc->vPartyId) free(stc->vPartyId);
        if (stc->vEphemeralData) free(stc->vEphemeralData);
        if (stc->providedDkm) free(stc->providedDkm);
        if (stc->outputDkm) free(stc->outputDkm);
        memzero_s(stc, sizeof(AMVP_KDA_TWOSTEP_TC));
    } else {
        return AMVP_UNSUPPORTED_OP;
    }
    return AMVP_SUCCESS;
}

static AMVP_KDA_TEST_TYPE read_test_type(const char *str) {
    int diff = 1;

    strcmp_s("AFT", 3, str, &diff);
    if (!diff) return AMVP_KDA_TT_AFT;

    strcmp_s("VAL", 3, str, &diff);
    if (!diff) return AMVP_KDA_TT_VAL;

    return 0;
}

AMVP_KDA_PATTERN_CANDIDATE cmp_pattern_str(AMVP_CTX *ctx, AMVP_CIPHER cipher, const char *str, AMVP_TEST_CASE *tc) {
    //size of (preprocessor string) includes null terminator
    AMVP_RESULT rv =  AMVP_SUCCESS;
    char *tmp = NULL, *lit = NULL, *token = NULL;
    rsize_t len = strnlen_s(str, AMVP_KDA_PATTERN_REG_STR_MAX + 1);
    int diff = 1;
    if (len > AMVP_KDA_PATTERN_REG_STR_MAX) { 
        AMVP_LOG_ERR("pattern candidate too long");
        return 0;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_UPARTYINFO_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_UPARTYINFO_STR) - 1) {
        return AMVP_KDA_PATTERN_UPARTYINFO;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_VPARTYINFO_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_VPARTYINFO_STR) - 1) {
        return AMVP_KDA_PATTERN_VPARTYINFO;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_CONTEXT_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_CONTEXT_STR) - 1) {
        return AMVP_KDA_PATTERN_CONTEXT;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_ALGID_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_ALGID_STR) - 1) {
        return AMVP_KDA_PATTERN_ALGID;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_LABEL_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_LABEL_STR) - 1) {
        return AMVP_KDA_PATTERN_LABEL;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_LENGTH_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_LENGTH_STR) - 1) {
        return AMVP_KDA_PATTERN_L;
    }
    strcmp_s(str, len, AMVP_KDA_PATTERN_T_STR, &diff);
    if (!diff && len == sizeof(AMVP_KDA_PATTERN_T_STR) - 1) {
        return AMVP_KDA_PATTERN_T;
    }
    //only compares first X number of characters, so should match, even though string is literal[0000000]
    if (sizeof(AMVP_KDA_PATTERN_LITERAL_STR) - 1 < len) {
        strncmp_s(str, len, AMVP_KDA_PATTERN_LITERAL_STR, sizeof(AMVP_KDA_PATTERN_LITERAL_STR) - 1, &diff);
        if (!diff) {
            //copy string so it can be tokenized
            tmp = calloc(len + 1, sizeof(char));
            if (!tmp) {
                AMVP_LOG_ERR("Failed to allocate memory when checking literal pattern");
                goto err;
            }
            strncpy_s(tmp, len + 1, str, len);

            //tokenize around the [] characters
            token = strtok_s(tmp, &len, "[", &lit);
            if (!token) { 
                AMVP_LOG_ERR("Invalid literal pattern candidate");
                goto err;
            }
            token = strtok_s(NULL, &len, "]", &lit); //the actual hex string
            if (!token) { 
                AMVP_LOG_ERR("Invalid literal pattern candidate");
                goto err;
            }
            if (strnlen_s(token, AMVP_KDA_PATTERN_LITERAL_STR_LEN_MAX + 1) > AMVP_KDA_PATTERN_LITERAL_STR_LEN_MAX) {
                AMVP_LOG_ERR("Patttern literal too long");
                goto err;
            }
            if (cipher == AMVP_KDA_HKDF) {
                tc->tc.kda_hkdf->literalCandidate = calloc(AMVP_KDA_PATTERN_LITERAL_BYTE_MAX, 1);
                if (!tc->tc.kda_hkdf->literalCandidate) {
                    AMVP_LOG_ERR("Failed to allocate memory when setting literal pattern");
                    goto err;
                }
                rv = amvp_hexstr_to_bin(token, tc->tc.kda_hkdf->literalCandidate, AMVP_KDA_PATTERN_LITERAL_BYTE_MAX, &(tc->tc.kda_hkdf->literalLen));
            } else {
                tc->tc.kda_onestep->literalCandidate = calloc(AMVP_KDA_PATTERN_LITERAL_BYTE_MAX, 1);
                if (!tc->tc.kda_onestep->literalCandidate) {
                    AMVP_LOG_ERR("Failed to allocate memory when setting literal pattern");
                    goto err;
                }
                rv = amvp_hexstr_to_bin(token, tc->tc.kda_onestep->literalCandidate, AMVP_KDA_PATTERN_LITERAL_BYTE_MAX, &(tc->tc.kda_onestep->literalLen));
            }
            if (rv != AMVP_SUCCESS) {
                AMVP_LOG_ERR("Hex conversion failure (literal candidate)");
                goto err;
            }
            if (tmp) free(tmp);
            return AMVP_KDA_PATTERN_LITERAL;
        }
    }

    AMVP_LOG_ERR("Candidate string provided does not match any valid candidate");
err:
    if (tmp) free(tmp);
    return 0;
}

static AMVP_KDA_PATTERN_CANDIDATE* read_info_pattern(AMVP_CTX *ctx, AMVP_CIPHER cipher, const char *str, AMVP_TEST_CASE *tc) {
    AMVP_KDA_PATTERN_CANDIDATE currentCand;
    char *cpy = NULL;
    AMVP_KDA_PATTERN_CANDIDATE *rv = NULL;
    int hasUParty = 0, hasVParty = 0; //Currently, these are required
    if (!str) {
        return NULL;
    }
    rsize_t len = strnlen_s(str, AMVP_KDA_PATTERN_REG_STR_MAX + 1);
    if (len > AMVP_KDA_PATTERN_REG_STR_MAX || len < 1) {
        return NULL;
    }
    cpy = calloc(AMVP_KDA_PATTERN_REG_STR_MAX + 1, sizeof(char));
    if (!cpy) {
        AMVP_LOG_ERR("Failed to allocate memory in temp string while reading info pattern");
        return NULL;
    }
    if (strncpy_s(cpy, AMVP_KDA_PATTERN_REG_STR_MAX + 1, str, AMVP_KDA_PATTERN_REG_STR_MAX)) {
        AMVP_LOG_ERR("Failed to copy string into temp holder for tokenization");

    }
    //we return this to be used in the test case struct, so we want it to survive the scope of this function
    AMVP_KDA_PATTERN_CANDIDATE *arr = calloc(AMVP_KDA_PATTERN_MAX, sizeof(int));
    if (!arr) {
        AMVP_LOG_ERR("Failed to allocate memory for reading fixedInfoPattern");
        goto err;
    }
    const char *token = NULL;
    char *tmp = NULL;
    token = strtok_s(cpy, &len, "||", &tmp);
    int count = 0;
    if (!token) {
        AMVP_LOG_ERR("Server JSON invalid 'fixedInfoPattern'");
        free(arr);
        goto err;
    } 

    do {
        if (count >= AMVP_KDA_PATTERN_MAX) {
            AMVP_LOG_ERR("Pattern string has too many elements");
            free(arr);
            goto err;
        }
        currentCand = cmp_pattern_str(ctx, cipher, token, tc);
        if (currentCand >= AMVP_KDA_PATTERN_MAX || currentCand <= AMVP_KDA_PATTERN_NONE) {
            AMVP_LOG_ERR("Invalid pattern candidate supplied by server JSON");
            free(arr);
            goto err;
        } else {
            if (currentCand == AMVP_KDA_PATTERN_UPARTYINFO) {
                hasUParty = 1;
            }
            if (currentCand == AMVP_KDA_PATTERN_VPARTYINFO) {
                hasVParty = 1;
            }
            arr[count] = currentCand;
        }
        count++;
        token = strtok_s(NULL, &len, "||", &tmp);
    } while(token);

    if (!hasUParty || !hasVParty) {
        free(arr);
        goto err;
    }
    rv = arr;
err:
    if (cpy) free(cpy);
    return rv;
}

static AMVP_KDA_ENCODING read_encoding_type(const char* str) {
    int diff = 1;
    if (!str) {
        return 0;
    }
    strncmp_s(AMVP_KDA_ENCODING_CONCATENATION_STR,
              sizeof(AMVP_KDA_ENCODING_CONCATENATION_STR) - 1,
              str, strnlen_s(str, 16), &diff);
            
    if (!diff) {
        return AMVP_KDA_ENCODING_CONCAT;
    }

    return 0;
}

static AMVP_KDA_MAC_SALT_METHOD read_salt_method(const char* str) {
    int diff = 1;
    if (!str) {
        return 0;
    }
    strncmp_s(AMVP_KDA_MAC_SALT_METHOD_DEFAULT_STR, 
              sizeof(AMVP_KDA_MAC_SALT_METHOD_DEFAULT_STR) - 1,
              str, strnlen_s(str, 16), &diff);
    if (!diff) {
        return AMVP_KDA_MAC_SALT_METHOD_DEFAULT;
    }
    strncmp_s(AMVP_KDA_MAC_SALT_METHOD_RANDOM_STR, 
              sizeof(AMVP_KDA_MAC_SALT_METHOD_RANDOM_STR) - 1,
              str, strnlen_s(str, 16), &diff);
    if (!diff) {
        return AMVP_KDA_MAC_SALT_METHOD_RANDOM;
    }
    return 0;
}

/*
 * This is used for both HKDF and OneStep. the only real difference is that hkdf
 * uses an hmac alg and onestep uses an aux function (which can also be an hmac alg)
 */
static AMVP_RESULT amvp_kda_process(AMVP_CTX *ctx,
                                        AMVP_CIPHER cipher, 
                                        AMVP_CAPS_LIST *cap,
                                        AMVP_TEST_CASE *tc,
                                        JSON_Object *obj,
                                        JSON_Array *r_garr) {
    JSON_Value *groupval = NULL;
    JSON_Object *groupobj = NULL;
    JSON_Object *configobj = NULL;
    JSON_Array *groups = NULL;
    JSON_Value *testval = NULL;
    JSON_Object *testobj = NULL, *paramobj = NULL;
    JSON_Array *tests, *r_tarr = NULL;
    JSON_Value *r_tval = NULL, *r_gval = NULL;  /* Response testval, groupval */
    JSON_Object *r_tobj = NULL, *r_gobj = NULL; /* Response testobj, groupobj */
    const char *alg_str = NULL,  *pattern_str = NULL, *encoding_str = NULL, 
               *salt_method_str = NULL;
    AMVP_HASH_ALG hmac_alg = 0;
    unsigned int i = 0, g_cnt = 0;
    int j = 0, k = 0, t_cnt = 0, tc_id = 0, saltLen = 0, l = 0,hybrid_secret = 0;
    AMVP_RESULT rv;
    const char *test_type_str = NULL;
    AMVP_KDA_TEST_TYPE test_type;
    AMVP_KDA_PATTERN_CANDIDATE *arr = NULL;
    AMVP_KDA_ENCODING encoding;
    AMVP_KDA_MAC_SALT_METHOD salt_method;
    AMVP_CAPS_LIST *kdfcap = NULL;
    /*These vars are specific to onestep */
    AMVP_CIPHER aux_function = 0;
    /* These vars are specific to twostep */
    int ctr_len = 0, iv_len = 0;
    const char *ctr_loc_str = NULL, *kdf_mode_str = NULL, *iv_str = NULL;
    AMVP_KDF108_MODE kdf_mode = 0;
    AMVP_KDF108_MAC_MODE_VAL mac_mode = 0;
    AMVP_KDF108_FIXED_DATA_ORDER_VAL ctr_loc = 0;

    groups = json_object_get_array(obj, "testGroups");
    g_cnt = json_array_get_count(groups);

    for (i = 0; i < g_cnt; i++) {
        int tgId = 0;
        groupval = json_array_get_value(groups, i);
        groupobj = json_value_get_object(groupval);
        if (!groupobj) {
            AMVP_LOG_ERR("Missing test group in server JSON");
        }

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
        test_type_str = json_object_get_string(groupobj, "testType");
        if (!test_type_str) {
            AMVP_LOG_ERR("Server JSON missing 'testType'");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
        test_type = read_test_type(test_type_str);
        if (!test_type) {
            AMVP_LOG_ERR("Server provided invalid testType");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
        json_object_set_number(r_gobj, "tgId", tgId);

        configobj = json_object_get_object(groupobj, "kdfConfiguration");
        if (!configobj) {
            AMVP_LOG_ERR("Missing kdfConfiguration object in server JSON");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
        if (cipher == AMVP_KDA_HKDF) {
            alg_str = json_object_get_string(configobj, "hmacAlg");
            if (!alg_str) {
                AMVP_LOG_ERR("Server JSON missing 'hashAlg'");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }

            hmac_alg = amvp_lookup_hash_alg(alg_str);
            switch (hmac_alg) {
            case AMVP_SHA1:
            case AMVP_SHA224:
            case AMVP_SHA256:
            case AMVP_SHA384:
            case AMVP_SHA512:
            case AMVP_SHA512_224:
            case AMVP_SHA512_256:
            case AMVP_SHA3_224:
            case AMVP_SHA3_256:
            case AMVP_SHA3_384:
            case AMVP_SHA3_512:
                break;
            case AMVP_NO_SHA:
            case AMVP_HASH_ALG_MAX:
            default:
                AMVP_LOG_ERR("Server JSON invalid 'hashAlg'");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
        } else if (cipher == AMVP_KDA_ONESTEP) {
            alg_str = json_object_get_string(configobj, "auxFunction");
            if (!alg_str) {
                AMVP_LOG_ERR("Server JSON missing 'auxFunction'");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
            aux_function = amvp_lookup_aux_function_alg_tbl(alg_str);
            if (!aux_function) {
                AMVP_LOG_ERR("Invalid auxFunction provided by server JSON");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
        } else if (cipher == AMVP_KDA_TWOSTEP) {
            alg_str = json_object_get_string(configobj, "macMode");
            if (!alg_str) {
                AMVP_LOG_ERR("Server JSON missing 'macMode'");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }
            mac_mode = read_mac_mode(alg_str);
            if (!mac_mode) {
                AMVP_LOG_ERR("Sever JSON invalid 'macMode'");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }
        } else {
            AMVP_LOG_ERR("Error, incorrect cipher in KDA handler");
            rv = AMVP_UNSUPPORTED_OP;
            goto err;
        }

        pattern_str = json_object_get_string(configobj, "fixedInfoPattern");
        if (!pattern_str) {
            AMVP_LOG_ERR("Server JSON missing 'fixedInfoPattern'");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }

        encoding_str = json_object_get_string(configobj, "fixedInfoEncoding");
        encoding = read_encoding_type(encoding_str);
        if (!encoding) {
            AMVP_LOG_ERR("Invalid fixedInfoEncoding provided by server");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
        salt_method_str = json_object_get_string(configobj, "saltMethod");
        salt_method = read_salt_method(salt_method_str);
        if (!salt_method) {
            AMVP_LOG_ERR("Invalid saltMethod provided by server");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }

        saltLen = json_object_get_number(configobj, "saltLen");
        if (saltLen % 8 != 0 || saltLen < 0 || saltLen > AMVP_KDA_SALT_BIT_MAX) {
            AMVP_LOG_ERR("Invalid saltLen provided by server");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }

        l = json_object_get_number(configobj, "l");
        if (cipher == AMVP_KDA_HKDF) {
            kdfcap = amvp_locate_cap_entry(ctx, AMVP_KDA_HKDF);
            if (!kdfcap || !kdfcap->cap.kda_hkdf_cap) {
                AMVP_LOG_ERR("Missing KDA_HKDF capability data");
                rv = AMVP_UNSUPPORTED_OP;
                goto err;
            }
            if (l != kdfcap->cap.kda_hkdf_cap->l || l <= 0 || l > AMVP_KDA_DKM_BIT_MAX) {
                AMVP_LOG_ERR("Server provided l is invalid");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
        } else if (cipher == AMVP_KDA_ONESTEP) {
            kdfcap = amvp_locate_cap_entry(ctx, AMVP_KDA_ONESTEP);
            if (!kdfcap || !kdfcap->cap.kda_onestep_cap) {
                AMVP_LOG_ERR("Missing KDA_ONESTEP capability data");
                rv = AMVP_UNSUPPORTED_OP;
                goto err;
            }
            if (l != kdfcap->cap.kda_onestep_cap->l || l < 0 || l > AMVP_KDA_DKM_BIT_MAX) {
                AMVP_LOG_ERR("Server provided l does not match registered value");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
        } else {
            kdfcap = amvp_locate_cap_entry(ctx, AMVP_KDA_TWOSTEP);
            if (!kdfcap || !kdfcap->cap.kda_twostep_cap) {
                AMVP_LOG_ERR("Missing KDA_TWOSTEP capability data");
                rv = AMVP_UNSUPPORTED_OP;
                goto err;
            }
            if (l != kdfcap->cap.kda_twostep_cap->l || l < 0 || l > AMVP_KDA_DKM_BIT_MAX) {
                AMVP_LOG_ERR("Server provided l does not match registered value");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
        }

        /* there are some kdfConfiguration values specific to twostep */
        if (cipher == AMVP_KDA_TWOSTEP) {
            kdf_mode_str = json_object_get_string(configobj, "kdfMode");
            if (!kdf_mode_str) {
                AMVP_LOG_ERR("Server JSON missing kdfMode");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }
            kdf_mode = read_mode(kdf_mode_str);
            if (!kdf_mode) {
                AMVP_LOG_ERR("Server JSON invalid kdfMode");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }

            ctr_loc_str = json_object_get_string(configobj, "counterLocation");
            if (!ctr_loc_str) {
                AMVP_LOG_ERR("Server JSON missing counterLocation");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }
            ctr_loc = read_ctr_location(ctr_loc_str);
            if (!ctr_loc) {
                AMVP_LOG_ERR("Server JSON invalid counterLocation.");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }

            ctr_len = json_object_get_number(configobj, "counterLen");
            if (ctr_len <= 0) {
                AMVP_LOG_ERR("Server JSON missing or invalid counterLen.");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }

            iv_len = json_object_get_number(configobj, "ivLen");
            if ((kdf_mode == AMVP_KDF108_MODE_COUNTER && !kdfcap->cap.kda_twostep_cap->kdf_params.counter_mode.requires_empty_iv)
            || (kdf_mode == AMVP_KDF108_MODE_DPI && !kdfcap->cap.kda_twostep_cap->kdf_params.dpi_mode.requires_empty_iv)
            || (kdf_mode == AMVP_KDF108_MODE_FEEDBACK && !kdfcap->cap.kda_twostep_cap->kdf_params.feedback_mode.requires_empty_iv)) {
                if (iv_len < 0) {
                    AMVP_LOG_ERR("Server JSON missing or invalid ivLen.");
                    rv = AMVP_TC_INVALID_DATA;
                    goto err;
                }
            } else if (iv_len > 0) {
                AMVP_LOG_ERR("Client registered requiring empty IV, but server sent non-zero ivLen");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }
        }

        /* in case of value not existing or being false, we have the same outcome */
        hybrid_secret = json_object_get_boolean(paramobj, "usesHybridSharedSecret");

        json_object_set_value(r_gobj, "tests", json_value_init_array());
        r_tarr = json_object_get_array(r_gobj, "tests");


        AMVP_LOG_VERBOSE("     Test group: %d", i);
        AMVP_LOG_VERBOSE("      test type: %s", test_type_str);
        if (cipher == AMVP_KDA_HKDF || cipher == AMVP_KDA_TWOSTEP) {
        AMVP_LOG_VERBOSE("            mac: %s", alg_str);
        } else if (cipher == AMVP_KDA_ONESTEP) {
        AMVP_LOG_VERBOSE("    auxFunction: %s", alg_str);
        }
        AMVP_LOG_VERBOSE("        pattern: %s", pattern_str);
        AMVP_LOG_VERBOSE("       encoding: %s", encoding_str);
        AMVP_LOG_VERBOSE("    salt method: %s", salt_method_str);
        AMVP_LOG_VERBOSE("       salt len: %d", saltLen);
        AMVP_LOG_VERBOSE("              l: %d", l);
        if (cipher == AMVP_KDA_TWOSTEP) {
        AMVP_LOG_VERBOSE("        kdfMode: %s", kdf_mode_str);
        AMVP_LOG_VERBOSE("counterLocation: %s", ctr_loc_str);
        AMVP_LOG_VERBOSE("     counterLen: %d", ctr_len);
        AMVP_LOG_VERBOSE("          ivLen: %d", iv_len);
        }

        tests = json_object_get_array(groupobj, "tests");
        t_cnt = json_array_get_count(tests);

        for (j = 0; j < t_cnt; j++) {
            JSON_Object *upartyobj = NULL, *vpartyobj = NULL;
            const char *salt = NULL, *z = NULL, *uparty = NULL,
                       *uephemeral = NULL, *vparty = NULL, 
                       *vephemeral = NULL, *dkm = NULL, *context = NULL,
                       *algid = NULL, *label = NULL, *t = NULL;
            AMVP_LOG_VERBOSE("Found new KDA test vector...");
            testval = json_array_get_value(tests, j);
            testobj = json_value_get_object(testval);
            paramobj = json_object_get_object(testobj, "kdfParameter");
            tc_id = json_object_get_number(testobj, "tcId");
            salt = json_object_get_string(paramobj, "salt");
          
            arr = read_info_pattern(ctx, cipher, pattern_str, tc);
            if (!arr || arr[0] <= AMVP_KDA_PATTERN_NONE || arr[0] > AMVP_KDA_PATTERN_MAX) {
                AMVP_LOG_ERR("Invalid fixedInfoPattern provided by server");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }

            //for onestep, salt only exists for HMAC aux functions
            if (cipher != AMVP_KDA_ONESTEP) {
                if (!salt) {
                    AMVP_LOG_ERR("Server JSON missing 'salt'");
                    rv = AMVP_MALFORMED_JSON;
                    goto err;
                }
                if ((int)strnlen_s(salt, AMVP_KDA_SALT_STR_MAX + 1) != saltLen / 4) {
                    AMVP_LOG_ERR("salt wrong length, should match provided saltLen %d",
                                saltLen);
                    rv = AMVP_MALFORMED_JSON;
                    goto err;
                }
            }

            z = json_object_get_string(paramobj, "z");
            if (!z) {
                AMVP_LOG_ERR("Server JSON missing 'z'");
                rv = AMVP_MALFORMED_JSON;
                goto err;
            }
            if (strnlen_s(z, AMVP_KDA_Z_STR_MAX + 1) > AMVP_KDA_Z_STR_MAX) {
                AMVP_LOG_ERR("Server JSON 'z' too long");
                rv = AMVP_TC_INVALID_DATA;
                goto err;
            }

            if (cipher == AMVP_KDA_TWOSTEP && iv_len > 0) {
                iv_str = json_object_get_string(paramobj, "iv");
                if (!iv_str) {
                    AMVP_LOG_ERR("Server JSON missing 'iv'");
                    rv = AMVP_TC_INVALID_DATA;
                    goto err;
                }
            }

            //Read the array of pattern candidates, read specific JSON objects based on whats there
            for (k = 0; k < AMVP_KDA_PATTERN_MAX; k++) {
                if (arr[k] >= AMVP_KDA_PATTERN_MAX || arr[k] <= AMVP_KDA_PATTERN_NONE) {
                    break;
                }
                switch (arr[k]) {                    
                case AMVP_KDA_PATTERN_UPARTYINFO:
                    upartyobj = json_object_get_object(testobj, "fixedInfoPartyU");
                    if (!upartyobj) {
                        AMVP_LOG_ERR("Server JSON missing 'fixedInfoPartyU'");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    uparty = json_object_get_string(upartyobj, "partyId");
                    if (!uparty) {
                        AMVP_LOG_ERR("Server JSON missing 'partyId' in 'fixedInfoPatyU");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    //ephemeral data is randomly included and optional
                    uephemeral = json_object_get_string(upartyobj, "ephemeralData");
                    break;
                case AMVP_KDA_PATTERN_VPARTYINFO:
                    vpartyobj = json_object_get_object(testobj, "fixedInfoPartyV");
                    if (!vpartyobj) {
                        AMVP_LOG_ERR("Server JSON missing 'fixedInfoPartyV'");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    vparty = json_object_get_string(vpartyobj, "partyId");
                    if (!vparty) {
                        AMVP_LOG_ERR("Server JSON missing 'partyId' in 'fixedInfoPatyU");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    //ephemeral data is randomly included and optional
                    vephemeral = json_object_get_string(vpartyobj, "ephemeralData");
                    break;
                case AMVP_KDA_PATTERN_CONTEXT:
                    context = json_object_get_string(paramobj, "context");
                    if (!context) {
                        AMVP_LOG_ERR("Server JSON missing 'context'");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    break;
                case AMVP_KDA_PATTERN_ALGID:
                    algid = json_object_get_string(paramobj, "algorithmId");
                    if (!algid) {
                        AMVP_LOG_ERR("Server JSON missing 'algorithmId'");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    break;
                case AMVP_KDA_PATTERN_LABEL:
                    label = json_object_get_string(paramobj, "label");
                    if (!label) {
                        AMVP_LOG_ERR("Server JSON missing 'label'");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    break;
                case AMVP_KDA_PATTERN_T:
                    t = json_object_get_string(paramobj, "t");
                    if (!t) {
                        AMVP_LOG_ERR("Server JSON missing 't'");
                        rv = AMVP_MALFORMED_JSON;
                        goto err;
                    }
                    break;
                case AMVP_KDA_PATTERN_MAX:
                case AMVP_KDA_PATTERN_L:
                case AMVP_KDA_PATTERN_LITERAL:
                case AMVP_KDA_PATTERN_NONE:
                default:
                    break;
                }
            }

            if (!t && json_object_has_value(paramobj, "t") && hybrid_secret) {
                t = json_object_get_string(paramobj, "t");
                if (!t) {
                    AMVP_LOG_ERR("Server JSON missing 't'");
                    rv = AMVP_MALFORMED_JSON;
                    goto err;
                }
            }

            if (test_type == AMVP_KDA_TT_VAL) {
                /*
                 * Validate
                 */
                dkm = json_object_get_string(testobj, "dkm");
                if (!dkm) {
                    AMVP_LOG_ERR("Server json missing 'dkm'");
                    rv = AMVP_MALFORMED_JSON;
                    goto err;
                }
                if ((int)strnlen_s(dkm, AMVP_KDA_DKM_STR_MAX) != l / 4) {
                    AMVP_LOG_ERR("Given dkm wrong length, should match provided l %d",  l);
                    rv = AMVP_MALFORMED_JSON;
                    goto err;
                }
            }
            AMVP_LOG_VERBOSE("           tcId: %d", tc_id);
            AMVP_LOG_VERBOSE("           salt: %s", salt);
            AMVP_LOG_VERBOSE("              z: %s", z);
            if (t) {
            AMVP_LOG_VERBOSE("              t: %s", t);
            }
            AMVP_LOG_VERBOSE("       uPartyId: %s", uparty);
            if (uephemeral) {
            AMVP_LOG_VERBOSE("     uEphemeral: %s", uephemeral);
            }
            AMVP_LOG_VERBOSE("       vPartyId: %s", vparty);
            if (vephemeral) {
            AMVP_LOG_VERBOSE("     vEphemeral: %s", vephemeral);
            }
            if (algid) {
            AMVP_LOG_VERBOSE("    algorithmId: %s", algid);
            }
            if (context) {
            AMVP_LOG_VERBOSE("        context: %s", context);
            }
            if (label) {
            AMVP_LOG_VERBOSE("          label: %s", label);
            }
            if (dkm) {
            AMVP_LOG_VERBOSE("            dkm: %s", dkm);
            }

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
            if (cipher == AMVP_KDA_HKDF) {
                rv = amvp_kda_hkdf_init_tc(ctx, tc->tc.kda_hkdf, tc_id, hmac_alg, hybrid_secret, salt, z, t, uparty, uephemeral,
                                            vparty, vephemeral, algid, context, label, dkm, l, saltLen,
                                            salt_method, encoding, arr, test_type);
            } else if (cipher == AMVP_KDA_ONESTEP) {
                rv = amvp_kda_onestep_init_tc(ctx, tc->tc.kda_onestep, tc_id, aux_function, salt, z, t, uparty, uephemeral,
                                                vparty, vephemeral, algid, context, label, dkm, l, saltLen,
                                                salt_method, encoding, arr, test_type);
            } else {
                rv = amvp_kda_twostep_init_tc(ctx, tc->tc.kda_twostep, tc_id, mac_mode, hybrid_secret, salt, z, iv_str, t, uparty,
                                                uephemeral, vparty, vephemeral, algid, context, label, dkm, l, saltLen, iv_len,
                                                ctr_len, salt_method, kdf_mode, ctr_loc, encoding, arr, test_type);
            }

            if (arr) free(arr);
            arr = NULL;

            if (rv != AMVP_SUCCESS) {
                if (cipher == AMVP_KDA_HKDF) {
                    amvp_kda_release_tc(AMVP_KDA_HKDF, tc);
                } else if (cipher == AMVP_KDA_ONESTEP) {
                    amvp_kda_release_tc(AMVP_KDA_ONESTEP, tc);
                } else {
                    amvp_kda_release_tc(AMVP_KDA_TWOSTEP, tc);
                }
                json_value_free(r_tval);
                goto err;
            }
            /* Process the current KAT test vector... */
            if ((cap->crypto_handler)(tc)) {
                if (cipher == AMVP_KDA_HKDF) {
                    amvp_kda_release_tc(AMVP_KDA_HKDF, tc);
                } else if (cipher == AMVP_KDA_ONESTEP) {
                    amvp_kda_release_tc(AMVP_KDA_ONESTEP, tc);
                } else {
                    amvp_kda_release_tc(AMVP_KDA_TWOSTEP, tc);
                }
                AMVP_LOG_ERR("crypto module failed the operation");
                rv = AMVP_CRYPTO_MODULE_FAIL;
                json_value_free(r_tval);
                goto err;
            }

            /*
             * Output the test case results using JSON
             */
            if (cipher == AMVP_KDA_HKDF) {
                rv = amvp_kda_hkdf_output_tc(ctx, tc->tc.kda_hkdf, r_tobj);
                if (rv != AMVP_SUCCESS) {
                    AMVP_LOG_ERR("JSON output failure in KDA module");
                    amvp_kda_release_tc(AMVP_KDA_HKDF, tc);
                    json_value_free(r_tval);
                    goto err;
                }
            } else if (cipher == AMVP_KDA_ONESTEP) {
                rv = amvp_kda_onestep_output_tc(ctx, tc->tc.kda_onestep, r_tobj);
                if (rv != AMVP_SUCCESS) {
                    AMVP_LOG_ERR("JSON output failure in KDA module");
                    amvp_kda_release_tc(AMVP_KDA_ONESTEP, tc);
                    json_value_free(r_tval);
                    goto err;
                }
            } else {
                rv = amvp_kda_twostep_output_tc(ctx, tc->tc.kda_twostep, r_tobj);
                if (rv != AMVP_SUCCESS) {
                    AMVP_LOG_ERR("JSON output failure in KDA module");
                    amvp_kda_release_tc(AMVP_KDA_TWOSTEP, tc);
                    json_value_free(r_tval);
                    goto err;
                }
            }

            /*
             * Release all the memory associated with the test case
             */
            if (cipher == AMVP_KDA_HKDF) {
                amvp_kda_release_tc(AMVP_KDA_HKDF, tc);
            } else if (cipher == AMVP_KDA_ONESTEP) {
                amvp_kda_release_tc(AMVP_KDA_ONESTEP, tc);
            } else {
                amvp_kda_release_tc(AMVP_KDA_TWOSTEP, tc);
            }

            /* Append the test response value to array */
            json_array_append_value(r_tarr, r_tval);
        }
        json_array_append_value(r_garr, r_gval);
        if (arr) { free(arr); arr = NULL; }
    }
    rv = AMVP_SUCCESS;

err:
    if (rv != AMVP_SUCCESS) {
        if (arr) free(arr);
        json_value_free(r_gval);
    }
    return rv;
}

AMVP_RESULT amvp_kda_hkdf_kat_handler(AMVP_CTX *ctx, JSON_Object *obj) {
    JSON_Value *r_vs_val = NULL;
    JSON_Object *r_vs = NULL;
    JSON_Array *r_garr = NULL; /* Response testarray */
    JSON_Value *reg_arry_val = NULL;
    JSON_Array *reg_arry = NULL;
    JSON_Object *reg_obj = NULL;
    AMVP_CAPS_LIST *cap;
    AMVP_TEST_CASE tc;
    AMVP_KDA_HKDF_TC stc;
    AMVP_RESULT rv = AMVP_SUCCESS;
    const char *alg_str = NULL;
    char *json_result = NULL;
    const char *mode_str = NULL;

    if (!ctx) {
        AMVP_LOG_ERR("No ctx for handler operation");
        return AMVP_NO_CTX;
    }

    alg_str = json_object_get_string(obj, "algorithm");
    if (!alg_str) {
        AMVP_LOG_ERR("unable to parse 'algorithm' from JSON");
        return AMVP_MALFORMED_JSON;
    }

    /*
    * Get a reference to the abstracted test case
    */
    tc.tc.kda_hkdf = &stc;
    memzero_s(&stc, sizeof(AMVP_KDA_HKDF_TC));

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
    mode_str = json_object_get_string(obj, "mode");
    json_object_set_string(r_vs, "mode", mode_str);

    if (mode_str) {
        stc.cipher = amvp_lookup_cipher_w_mode_index(alg_str, mode_str);
        if (stc.cipher != AMVP_KDA_HKDF) {
            AMVP_LOG_ERR("Server JSON invalid 'algorithm' or 'mode'");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
    } else {
        AMVP_LOG_ERR("Missing 'mode' in server JSON");
        rv = AMVP_MALFORMED_JSON;
        goto err;
    }
    if (stc.cipher != AMVP_KDA_HKDF) {
        AMVP_LOG_ERR("Invalid cipher for KDA-HKDF handler");
        rv = AMVP_INVALID_ARG;
        goto err;
    }

    cap = amvp_locate_cap_entry(ctx, AMVP_KDA_HKDF);
    if (!cap) {
        AMVP_LOG_ERR("AMVP server requesting unsupported capability");
        rv = AMVP_UNSUPPORTED_OP;
        goto err;
    }
    rv = amvp_kda_process(ctx, AMVP_KDA_HKDF, cap, &tc, obj, r_garr);
    if (rv != AMVP_SUCCESS) {
        goto err;
    }

    json_array_append_value(reg_arry, r_vs_val);

    json_result = json_serialize_to_string_pretty(ctx->kat_resp, NULL);
    AMVP_LOG_VERBOSE("\n\n%s\n\n", json_result);
    json_free_serialized_string(json_result);
    rv = AMVP_SUCCESS;

err:
    if (rv != AMVP_SUCCESS) {
        amvp_kda_release_tc(AMVP_KDA_HKDF, &tc);
        json_value_free(r_vs_val);
    }
    return rv;
}

AMVP_RESULT amvp_kda_onestep_kat_handler(AMVP_CTX *ctx, JSON_Object *obj) {
    JSON_Value *r_vs_val = NULL;
    JSON_Object *r_vs = NULL;
    JSON_Array *r_garr = NULL; /* Response testarray */
    JSON_Value *reg_arry_val = NULL;
    JSON_Array *reg_arry = NULL;
    JSON_Object *reg_obj = NULL;
    AMVP_CAPS_LIST *cap;
    AMVP_TEST_CASE tc;
    AMVP_KDA_ONESTEP_TC stc;
    AMVP_RESULT rv = AMVP_SUCCESS;
    const char *alg_str = NULL;
    char *json_result = NULL;
    const char *mode_str = NULL;

    if (!ctx) {
        AMVP_LOG_ERR("No ctx for handler operation");
        return AMVP_NO_CTX;
    }

    alg_str = json_object_get_string(obj, "algorithm");
    if (!alg_str) {
        AMVP_LOG_ERR("unable to parse 'algorithm' from JSON");
        return AMVP_MALFORMED_JSON;
    }

    /*
    * Get a reference to the abstracted test case
    */
    tc.tc.kda_onestep = &stc;
    memzero_s(&stc, sizeof(AMVP_KDA_ONESTEP_TC));

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
    mode_str = json_object_get_string(obj, "mode");
    json_object_set_string(r_vs, "mode", mode_str);

    if (mode_str) {
        stc.cipher = amvp_lookup_cipher_w_mode_index(alg_str, mode_str);
        if (stc.cipher != AMVP_KDA_ONESTEP) {
            AMVP_LOG_ERR("Server JSON invalid 'algorithm' or 'mode'");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
    } else {
        AMVP_LOG_ERR("Missing 'mode' in server JSON");
        rv = AMVP_MALFORMED_JSON;
        goto err;
    }
    if (stc.cipher != AMVP_KDA_ONESTEP) {
        AMVP_LOG_ERR("Invalid cipher for KDA-ONESTEP handler");
        rv = AMVP_MALFORMED_JSON;
        goto err;
    }

    cap = amvp_locate_cap_entry(ctx, AMVP_KDA_ONESTEP);
    if (!cap) {
        AMVP_LOG_ERR("AMVP server requesting unsupported capability");
        rv = AMVP_UNSUPPORTED_OP;
        goto err;
    }
    rv = amvp_kda_process(ctx, AMVP_KDA_ONESTEP, cap, &tc, obj, r_garr);
    if (rv != AMVP_SUCCESS) {
        goto err;
    }

    json_array_append_value(reg_arry, r_vs_val);

    json_result = json_serialize_to_string_pretty(ctx->kat_resp, NULL);
    AMVP_LOG_VERBOSE("\n\n%s\n\n", json_result);
    json_free_serialized_string(json_result);
    rv = AMVP_SUCCESS;

err:
    if (rv != AMVP_SUCCESS) {
        amvp_kda_release_tc(AMVP_KDA_ONESTEP, &tc);
        json_value_free(r_vs_val);
    }
    return rv;
}

AMVP_RESULT amvp_kda_twostep_kat_handler(AMVP_CTX *ctx, JSON_Object *obj) {
    JSON_Value *r_vs_val = NULL;
    JSON_Object *r_vs = NULL;
    JSON_Array *r_garr = NULL; /* Response testarray */
    JSON_Value *reg_arry_val = NULL;
    JSON_Array *reg_arry = NULL;
    JSON_Object *reg_obj = NULL;
    AMVP_CAPS_LIST *cap;
    AMVP_TEST_CASE tc;
    AMVP_KDA_TWOSTEP_TC stc;
    AMVP_RESULT rv = AMVP_SUCCESS;
    const char *alg_str = NULL;
    char *json_result = NULL;
    const char *mode_str = NULL;

    if (!ctx) {
        AMVP_LOG_ERR("No ctx for handler operation");
        return AMVP_NO_CTX;
    }

    alg_str = json_object_get_string(obj, "algorithm");
    if (!alg_str) {
        AMVP_LOG_ERR("unable to parse 'algorithm' from JSON");
        return AMVP_MALFORMED_JSON;
    }

    /*
    * Get a reference to the abstracted test case
    */
    tc.tc.kda_twostep = &stc;
    memzero_s(&stc, sizeof(AMVP_KDA_TWOSTEP_TC));

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
    mode_str = json_object_get_string(obj, "mode");
    json_object_set_string(r_vs, "mode", mode_str);

    if (mode_str) {
        stc.cipher = amvp_lookup_cipher_w_mode_index(alg_str, mode_str);
        if (stc.cipher != AMVP_KDA_TWOSTEP) {
            AMVP_LOG_ERR("Server JSON invalid 'algorithm' or 'mode'");
            rv = AMVP_MALFORMED_JSON;
            goto err;
        }
    } else {
        AMVP_LOG_ERR("Missing 'mode' in server JSON");
        rv = AMVP_MALFORMED_JSON;
        goto err;
    }
    if (stc.cipher != AMVP_KDA_TWOSTEP) {
        AMVP_LOG_ERR("Invalid cipher for KDA-TWOSTEP handler");
        rv = AMVP_MALFORMED_JSON;
        goto err;
    }

    cap = amvp_locate_cap_entry(ctx, AMVP_KDA_TWOSTEP);
    if (!cap) {
        AMVP_LOG_ERR("AMVP server requesting unsupported capability");
        rv = AMVP_UNSUPPORTED_OP;
        goto err;
    }
    rv = amvp_kda_process(ctx, AMVP_KDA_TWOSTEP, cap, &tc, obj, r_garr);
    if (rv != AMVP_SUCCESS) {
        goto err;
    }

    json_array_append_value(reg_arry, r_vs_val);

    json_result = json_serialize_to_string_pretty(ctx->kat_resp, NULL);
    AMVP_LOG_VERBOSE("\n\n%s\n\n", json_result);
    json_free_serialized_string(json_result);
    rv = AMVP_SUCCESS;

err:
    if (rv != AMVP_SUCCESS) {
        amvp_kda_release_tc(AMVP_KDA_TWOSTEP, &tc);
        json_value_free(r_vs_val);
    }
    return rv;
}
