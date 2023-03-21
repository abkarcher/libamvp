/*
 * Copyright (c) 2021, Cisco Systems, Inc.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/cisco/libamvp/LICENSE
 */


#include <openssl/evp.h>
#include "amvp/amvp.h"
#include "app_lcl.h"
#include "safe_lib.h"
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#endif


static EVP_CIPHER_CTX *glb_cipher_ctx = NULL; /* need to maintain across calls for MCT */

void app_aes_cleanup(void) {
    if (glb_cipher_ctx) EVP_CIPHER_CTX_free(glb_cipher_ctx);
    glb_cipher_ctx = NULL;
}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L

int app_aes_handler(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC  *tc = NULL;
    EVP_CIPHER_CTX *cipher_ctx = NULL;
    EVP_CIPHER *cipher = NULL;
    const char *type = NULL, *mode = NULL;
    char alg_name[32];
    unsigned char *iv = NULL;
    int rv = 1, direction = 0;
    AMVP_SUB_AES alg;
    OSSL_PARAM_BLD *pbld = NULL;
    OSSL_PARAM *params = NULL;

    if (!test_case) {
        return rv;
    }

    tc = test_case->tc.symmetric;

    if (glb_cipher_ctx == NULL) {
        glb_cipher_ctx = EVP_CIPHER_CTX_new();
        if (glb_cipher_ctx == NULL) {
            printf("Failed to allocate global cipher_ctx");
            return 1;
        }
    }

    /* Begin encrypt code section */
    cipher_ctx = glb_cipher_ctx;
    if ((tc->test_type != AMVP_SYM_TEST_TYPE_MCT)) {
        EVP_CIPHER_CTX_init(cipher_ctx);
    }

    alg = amvp_get_aes_alg(tc->cipher);
    if (alg == 0) {
        printf("Invalid cipher value");
        goto err;
    }

    switch (tc->key_len) {
    case 128:
        type = "AES-128-";
        break;
    case 192:
        type = "AES-192-";
        break;
    case 256:
        type = "AES-256-";
        break;
    default:
        printf("Unsupported AES key length\n");
        rv = 1;
        goto err;
    }

    switch (alg) {
    case AMVP_SUB_AES_ECB:
        mode = "ECB";
        break;
    case AMVP_SUB_AES_CTR:
        mode = "CTR";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_CFB1:
        mode = "CFB1";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_CFB8:
        mode = "CFB8";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_CFB128:
        mode = "CFB";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_OFB:
        mode = "OFB";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_CBC:
        mode = "CBC";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_CBC_CS1:
    case AMVP_SUB_AES_CBC_CS2:
    case AMVP_SUB_AES_CBC_CS3:
        mode = "CBC-CTS";
        iv = tc->iv;
        break;
    case AMVP_SUB_AES_XTS:
        mode = "XTS";
        if (tc->tw_mode == AMVP_SYM_CIPH_TWEAK_HEX) {
            iv = tc->iv;
        }
        break;
    case AMVP_SUB_AES_GCM:
    case AMVP_SUB_AES_GCM_SIV:
    case AMVP_SUB_AES_CCM:
    case AMVP_SUB_AES_XPN:
    case AMVP_SUB_AES_KW:
    case AMVP_SUB_AES_KWP:
    case AMVP_SUB_AES_GMAC:
    default:
        printf("Error: Unsupported AES mode requested by AMVP server\n");
        rv = 1;
        goto err;
    }

    memzero_s(alg_name, 32);
    strncat_s(alg_name, 32, type, strnlen_s(type, 16));
    strncat_s(alg_name, 32, mode, strnlen_s(mode, 16));

    cipher = EVP_CIPHER_fetch(NULL, alg_name, NULL);
    if (!cipher) {
        printf("Unable to fetch AES cipher\n");
        goto err;
    }

    if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
        direction = 1;
    }

    /* If Monte Carlo we need to be able to init and then update
     * one thousand times before we complete each iteration.
     */
    if (tc->test_type == AMVP_SYM_TEST_TYPE_MCT) {
        if (tc->mct_index == 0) {
            pbld = OSSL_PARAM_BLD_new();
            if (!pbld) {
                printf("Error creating param_bld in AES\n");
                goto err;
            }
            OSSL_PARAM_BLD_push_uint(pbld, OSSL_CIPHER_PARAM_PADDING, 0);
            if (tc->cipher == AMVP_AES_CFB1) {
                OSSL_PARAM_BLD_push_uint(pbld, OSSL_CIPHER_PARAM_USE_BITS, 1);
            }
            if (tc->cipher == AMVP_AES_CBC_CS2) {
                OSSL_PARAM_BLD_push_utf8_string(pbld, OSSL_CIPHER_PARAM_CTS_MODE, "CS2", 0);
            }
            if (tc->cipher == AMVP_AES_CBC_CS3) {
                OSSL_PARAM_BLD_push_utf8_string(pbld, OSSL_CIPHER_PARAM_CTS_MODE, "CS3", 0);
            }
            params = OSSL_PARAM_BLD_to_param(pbld);
            if (!params) {
                printf("Error generating params in AES\n");
                goto err;
            }
            if (EVP_CipherInit_ex2(cipher_ctx, cipher, tc->key, iv, direction, params) != 1) {
                printf("Error initializing MCT cipher CTX\n");
                goto err;
            }
        }

        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            EVP_CipherUpdate(cipher_ctx, tc->ct, (int *)&tc->ct_len, tc->pt, tc->pt_len);
            EVP_CipherFinal_ex(cipher_ctx, tc->ct, (int *)&tc->ct_len);
            tc->ct_len = tc->pt_len;
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            EVP_CipherUpdate(cipher_ctx, tc->pt, (int *)&tc->pt_len, tc->ct, tc->ct_len);
            EVP_CipherFinal_ex(cipher_ctx, tc->pt, (int *)&tc->pt_len);
            tc->pt_len = tc->ct_len;
        } else {
            printf("Unsupported direction\n");
            goto err;
        }
        if (tc->mct_index == AMVP_AES_MCT_INNER - 1) {
            EVP_CIPHER_CTX_free(cipher_ctx);
            glb_cipher_ctx = NULL;
        }
    } else {
        pbld = OSSL_PARAM_BLD_new();
        if (!pbld) {
            printf("Error creating param_bld in AES\n");
            goto err;
        }
        OSSL_PARAM_BLD_push_uint(pbld, OSSL_CIPHER_PARAM_PADDING, 0);
        if (tc->cipher == AMVP_AES_CFB1) {
            OSSL_PARAM_BLD_push_uint(pbld, OSSL_CIPHER_PARAM_USE_BITS, 1);
        }
        if (tc->cipher == AMVP_AES_CBC_CS2) {
            OSSL_PARAM_BLD_push_utf8_string(pbld, OSSL_CIPHER_PARAM_CTS_MODE, "CS2", 0);
        }
        if (tc->cipher == AMVP_AES_CBC_CS3) {
            OSSL_PARAM_BLD_push_utf8_string(pbld, OSSL_CIPHER_PARAM_CTS_MODE, "CS3", 0);
        }
        params = OSSL_PARAM_BLD_to_param(pbld);
        if (!params) {
            printf("Error generating params in AES\n");
            goto err;
        }
        if (EVP_CipherInit_ex2(cipher_ctx, cipher, tc->key, iv, direction, params) != 1) {
            printf("Error initializing cipher in AES\n");
            goto err;
        }

        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            if (EVP_CipherUpdate(cipher_ctx, tc->ct, (int *)&tc->ct_len, tc->pt, tc->pt_len) != 1) {
                printf("Error performing encrypt in AES\n");
                goto err;
            }
            if (EVP_CipherFinal_ex(cipher_ctx, tc->ct, (int *)&tc->ct_len) != 1) {
                printf("Error finalizing encrypt in AES\n");
                goto err;
            }
            tc->ct_len = tc->pt_len;
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            if (EVP_CipherUpdate(cipher_ctx, tc->pt, (int *)&tc->pt_len, tc->ct, tc->ct_len) != 1) {
                printf("Error performing decrypt in AES\n");
                goto err;
            }
            if (EVP_CipherFinal_ex(cipher_ctx, tc->pt, (int *)&tc->pt_len) != 1) {
                printf("Error finalizing decrypt in AES\n");
                goto err;
            }
            tc->pt_len = tc->ct_len;
        } else {
            printf("Unsupported direction\n");
            goto err;
        }

        EVP_CIPHER_CTX_free(cipher_ctx);
        glb_cipher_ctx = NULL;
    }
    rv = 0;

err:
    if (rv != 0) {
        if (glb_cipher_ctx) EVP_CIPHER_CTX_free(glb_cipher_ctx);
        glb_cipher_ctx = NULL;
    }
    if (cipher) EVP_CIPHER_free(cipher);
    if (pbld) OSSL_PARAM_BLD_free(pbld);
    if (params) OSSL_PARAM_free(params);
    return rv;
}

int app_aes_keywrap_handler(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC *tc;
    EVP_CIPHER_CTX *cipher_ctx = NULL;
    EVP_CIPHER *cipher = NULL;
    int out_len;
    int rv = 1;
    AMVP_SUB_AES alg;
    char alg_name[32];
    const char *type = NULL, *mode = NULL, *inv = NULL;

    if (!test_case) {
        return rv;
    }

    tc = test_case->tc.symmetric;

    /* Begin encrypt code section */
    cipher_ctx = EVP_CIPHER_CTX_new();
    if (!cipher_ctx) {
        printf("Error creating CTX in AES keywrap\n");
        goto err;
    }
    EVP_CIPHER_CTX_init(cipher_ctx);

    alg = amvp_get_aes_alg(tc->cipher);
    if (alg == 0) {
        printf("Invalid cipher value");
        goto err;
    }

    switch (tc->key_len) {
    case 128:
        type = "AES-128-";
        break;
    case 192:
        type = "AES-192-";
        break;
    case 256:
        type = "AES-256-";
        break;
    default:
        printf("Unsupported AES key length\n");
        rv = 1;
        goto err;
    }

    switch (alg) {
    case AMVP_SUB_AES_KW:
        mode = "WRAP";
        break;
    case AMVP_SUB_AES_KWP:
        mode = "WRAP-PAD";
        break;
    case AMVP_SUB_AES_ECB:
    case AMVP_SUB_AES_CTR:
    case AMVP_SUB_AES_CFB1:
    case AMVP_SUB_AES_CFB8:
    case AMVP_SUB_AES_CFB128:
    case AMVP_SUB_AES_OFB:
    case AMVP_SUB_AES_CBC:
    case AMVP_SUB_AES_CBC_CS1:
    case AMVP_SUB_AES_CBC_CS2:
    case AMVP_SUB_AES_CBC_CS3:
    case AMVP_SUB_AES_XTS:
    case AMVP_SUB_AES_GCM:
    case AMVP_SUB_AES_GCM_SIV:
    case AMVP_SUB_AES_CCM:
    case AMVP_SUB_AES_XPN:
    case AMVP_SUB_AES_GMAC:
    default:
        printf("Error: Unsupported AES mode requested by AMVP server\n");
        rv = 1;
        goto err;
    }

    if (tc->kwcipher == AMVP_SYM_KW_INVERSE) {
        inv = "-INV";
    }
    memzero_s(alg_name, 32);
    strncat_s(alg_name, 32, type, strnlen_s(type, 16));
    strncat_s(alg_name, 32, mode, strnlen_s(mode, 16));
    if (inv) {
        strncat_s(alg_name, 32, inv, strnlen_s(inv, 16));
    }

    cipher = EVP_CIPHER_fetch(NULL, alg_name, NULL);
    if (!cipher) {
        printf("Unable to fetch AES cipher\n");
        goto err;
    }

    if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
        EVP_CipherInit_ex2(cipher_ctx, cipher, tc->key, NULL, 1, NULL);
        EVP_CipherUpdate(cipher_ctx, tc->ct, &out_len, tc->pt, tc->pt_len);
        tc->ct_len = out_len;
        EVP_CipherFinal_ex(cipher_ctx, tc->ct, &out_len);
    } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
        EVP_CipherInit_ex2(cipher_ctx, cipher, tc->key, NULL, 0, NULL);
        if (EVP_CipherUpdate(cipher_ctx, tc->pt, &out_len, tc->ct, tc->ct_len) != 1) {
            goto err; /* return failure, some are expected for decrypt */
        }
        tc->pt_len = out_len;
        EVP_CipherFinal_ex(cipher_ctx, tc->pt, &out_len);
    } else {
        printf("Unsupported direction\n");
        goto err;
    }
    rv = 0;
err:
    /* Cleanup */
    if (cipher_ctx) EVP_CIPHER_CTX_free(cipher_ctx);
    if (cipher) EVP_CIPHER_free(cipher);
    return rv;
}

int app_aes_handler_aead(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC *tc;
    EVP_CIPHER_CTX *cipher_ctx = NULL;
    EVP_CIPHER *cipher = NULL;
    unsigned char iv_fixed[4] = { 1, 2, 3, 4 };
    int rc = 0, ret = 0, direction = 0;
    AMVP_SUB_AES alg;
    const char *alg_name = NULL;

    if (!test_case) {
        return 1;
    }

    tc = test_case->tc.symmetric;

    if (tc->direction != AMVP_SYM_CIPH_DIR_ENCRYPT && tc->direction != AMVP_SYM_CIPH_DIR_DECRYPT) {
        printf("Unsupported direction\n");
        return 1;
    }

    /* Begin encrypt code section */
    cipher_ctx = EVP_CIPHER_CTX_new();
    if (!cipher_ctx) {
        printf("Error initializing cipher CTX\n");
        rc = 1;
        goto err;
    }
    EVP_CIPHER_CTX_init(cipher_ctx);

    /* Validate key length and assign OpenSSL EVP cipher */
    alg = amvp_get_aes_alg(tc->cipher);
    if (alg == 0) {
        printf("Invalid cipher value");
        goto err;
    }

    switch (alg) {
    case AMVP_SUB_AES_GCM:
        switch (tc->key_len) {
        case 128:
            alg_name = "AES-128-GCM";
            break;
        case 192:
            alg_name = "AES-192-GCM";
            break;
        case 256:
            alg_name = "AES-256-GCM";
            break;
        default:
            printf("Unsupported AES-GCM key length\n");
            rc = 1;
            goto err;
        }

        cipher = EVP_CIPHER_fetch(NULL, alg_name, NULL);
        if (!cipher) {
            printf("Error fetching cipher in AES-GCM\n");
            goto err;
        }
        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            direction = 1;
        }

        EVP_CipherInit(cipher_ctx, cipher, NULL, NULL, direction);
        EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IVLEN, tc->iv_len, 0);
        if (tc->ivgen_source == AMVP_SYM_CIPH_IVGEN_SRC_INT) {
            EVP_CipherInit(cipher_ctx, NULL, tc->key, NULL, direction);
            if (direction) {
                EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IV_FIXED, 4, iv_fixed);
            } else {
                EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IV_FIXED, -1, tc->iv);
            }
            if (!EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_IV_GEN, tc->iv_len, tc->iv)) {
                printf("amvp_aes_encrypt: iv gen error\n");
                rc = 1;
                goto err;
            }
        } else {
            EVP_CipherInit(cipher_ctx, NULL, tc->key, tc->iv, direction);
        }

        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            if (tc->aad_len) {
                EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            }
            EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
            EVP_Cipher(cipher_ctx, NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_GET_TAG, tc->tag_len, tc->tag);
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            if (tc->aad_len) {
                EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_TAG, tc->tag_len, tc->tag);
                EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            }
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_TAG, tc->tag_len, tc->tag);
            EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
            ret = EVP_Cipher(cipher_ctx, NULL, NULL, 0);
            if (ret) {
                rc = 1;
                goto err;
            }
        }

        break;
    case AMVP_SUB_AES_CCM:
        switch (tc->key_len) {
        case 128:
            alg_name = "AES-128-CCM";
            break;
        case 192:
            alg_name = "AES-192-CCM";
            break;
        case 256:
            alg_name = "AES-256-CCM";
            break;
        default:
            printf("Unsupported AES-CCM key length\n");
            rc = 1;
            goto err;
        }

        cipher = EVP_CIPHER_fetch(NULL, alg_name, NULL);
        if (!cipher) {
            printf("Error fetching cipher in AES-CCM\n");
            goto err;
        }

        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            EVP_CipherInit(cipher_ctx, cipher, NULL, NULL, 1);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_IVLEN, tc->iv_len, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_TAG, tc->tag_len, 0);
            EVP_CipherInit(cipher_ctx, NULL, tc->key, tc->iv, 1);
            EVP_Cipher(cipher_ctx, NULL, NULL, tc->pt_len);
            EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            ret = EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
            if (ret < 0) {
                printf("Error performing encrypt operation CCM\n");
                rc = 1;
                goto err;
            }
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_GET_TAG, tc->tag_len, tc->ct + tc->ct_len);
            tc->ct_len += tc->tag_len;
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            EVP_CipherInit(cipher_ctx, cipher, NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_IVLEN, tc->iv_len, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_TAG, tc->tag_len, tc->ct + tc->pt_len);
            EVP_CipherInit(cipher_ctx, NULL, tc->key, tc->iv, 0);
            EVP_Cipher(cipher_ctx, NULL, NULL, tc->pt_len);
            EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);

            ret = EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
            if (ret < 0) {
                rc = 1;
                goto err;
            }
        }
        break;
    case AMVP_SUB_AES_GMAC: /* Has its own APIs */
    case AMVP_SUB_AES_GCM_SIV:
    case AMVP_SUB_AES_ECB:
    case AMVP_SUB_AES_CBC:
    case AMVP_SUB_AES_CFB1:
    case AMVP_SUB_AES_CFB8:
    case AMVP_SUB_AES_CFB128:
    case AMVP_SUB_AES_OFB:
    case AMVP_SUB_AES_CTR:
    case AMVP_SUB_AES_XTS:
    case AMVP_SUB_AES_KW:
    case AMVP_SUB_AES_KWP:
    case AMVP_SUB_AES_XPN:
    case AMVP_SUB_AES_CBC_CS1:
    case AMVP_SUB_AES_CBC_CS2:
    case AMVP_SUB_AES_CBC_CS3:
    default:
        printf("Error: Unsupported AES AEAD mode requested by AMVP server\n");
        rc = 1;
        goto err;
    }

err:
    /* Cleanup */
    if (cipher_ctx) EVP_CIPHER_CTX_free(cipher_ctx);
    if (cipher) EVP_CIPHER_free(cipher);
    return rc;
}

int app_aes_handler_gmac(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC *tc = NULL;
    EVP_MAC *mac = NULL;
    EVP_MAC_CTX *gmac_ctx = NULL;
    OSSL_PARAM_BLD *pbld = NULL;
    OSSL_PARAM *params = NULL;
    const char *alg_name = NULL;
    unsigned char *out = NULL;
    int rv = 1, diff = 1;

    if (!test_case) {
        return rv;
    }

    tc = test_case->tc.symmetric;
    if (!tc) return rv;

    switch (tc->key_len) {
    case 128:
        alg_name = "AES-128-GCM";
        break;
    case 192:
        alg_name = "AES-192-GCM";
        break;
    case 256:
        alg_name = "AES-256-GCM";
        break;
    default:
        printf("Unsupported AES-GMAC key length\n");
        goto err;
    }

    mac = EVP_MAC_fetch(NULL, "GMAC", NULL);
    if (!mac) {
        printf("Error: unable to fetch HMAC");
        goto err;
    }
    gmac_ctx = EVP_MAC_CTX_new(mac);
    if (!gmac_ctx) {
        printf("Error: unable to create HMAC CTX");
        goto err;
    }

    pbld = OSSL_PARAM_BLD_new();
    if (!pbld) {
        printf("Error creating param_bld in AES-GMAC\n");
        goto err;
    }
    OSSL_PARAM_BLD_push_utf8_string(pbld, OSSL_MAC_PARAM_CIPHER, alg_name, 0);
    OSSL_PARAM_BLD_push_octet_string(pbld, OSSL_MAC_PARAM_IV, tc->iv, tc->iv_len);
    params = OSSL_PARAM_BLD_to_param(pbld);
    if (!params) {
        printf("Error generating params in AES-GMAC\n");
        goto err;
    }

#define GMAC_BUF_MAX 16

    if (EVP_MAC_init(gmac_ctx, tc->key, tc->key_len / 8, params) != 1) {
        printf("Error in AES-GMAC, EVP_MAC_init failed\n");
        goto err;
    }

    if (EVP_MAC_update(gmac_ctx, tc->aad, tc->aad_len) != 1) {
        printf("Error in AES-GMAC, EVP_MAC_update failed\n");
        goto err;
    }

    /* Don't capture the output length; manually truncate for shorter tags */
    if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
        out = calloc(GMAC_BUF_MAX, sizeof(unsigned char));
        if (!out) {
            printf("Error allocating memory for AES-GMAC\n");
            goto err;
        }
        if (EVP_MAC_final(gmac_ctx, out, NULL, GMAC_BUF_MAX) != 1) {
            printf("Error in AES-GMAC, EVP_MAC_final failed\n");
            goto err;
        }

        memcmp_s(tc->tag, tc->tag_len, out, tc->tag_len, &diff);
        if (diff == 0) {
            rv = 0;
        }
    } else {
        if (EVP_MAC_final(gmac_ctx, tc->tag, NULL, GMAC_BUF_MAX) != 1) {
            printf("Error in AES-GMAC, EVP_MAC_final failed\n");
            goto err;
        }
        rv = 0;
    }

err:
    if (pbld) OSSL_PARAM_BLD_free(pbld);
    if (params) OSSL_PARAM_free(params);
    if (gmac_ctx) EVP_MAC_CTX_free(gmac_ctx);
    if (mac) EVP_MAC_free(mac);
    if (out) free(out);
    return rv;
}

#else

int app_aes_handler(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC  *tc = NULL;
    EVP_CIPHER_CTX *cipher_ctx = NULL;
    const EVP_CIPHER  *cipher = NULL;
    unsigned char *iv = 0;
    /* assume fail at first */
    int rv = 0;
    AMVP_SUB_AES alg;

    if (!test_case) {
        return rv;
    }

    tc = test_case->tc.symmetric;

    if (glb_cipher_ctx == NULL) {
        glb_cipher_ctx = EVP_CIPHER_CTX_new();
        if (glb_cipher_ctx == NULL) {
            printf("Failed to allocate global cipher_ctx");
            return 1;
        }
    }

    /* Begin encrypt code section */
    cipher_ctx = glb_cipher_ctx;
    if ((tc->test_type != AMVP_SYM_TEST_TYPE_MCT)) {
        EVP_CIPHER_CTX_init(cipher_ctx);
    }

    alg = amvp_get_aes_alg(tc->cipher);
    if (alg == 0) {
        printf("Invalid cipher value");
        goto err;
    }
    switch (alg) {
    case AMVP_SUB_AES_ECB:
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_ecb();
            break;
        case 192:
            cipher = EVP_aes_192_ecb();
            break;
        case 256:
            cipher = EVP_aes_256_ecb();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_CTR:
        iv = tc->iv;
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_ctr();
            break;
        case 192:
            cipher = EVP_aes_192_ctr();
            break;
        case 256:
            cipher = EVP_aes_256_ctr();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_CFB1:
        iv = tc->iv;
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_cfb1();
            break;
        case 192:
            cipher = EVP_aes_192_cfb1();
            break;
        case 256:
            cipher = EVP_aes_256_cfb1();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_CFB8:
        iv = tc->iv;
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_cfb8();
            break;
        case 192:
            cipher = EVP_aes_192_cfb8();
            break;
        case 256:
            cipher = EVP_aes_256_cfb8();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_CFB128:
        iv = tc->iv;
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_cfb128();
            break;
        case 192:
            cipher = EVP_aes_192_cfb128();
            break;
        case 256:
            cipher = EVP_aes_256_cfb128();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_OFB:
        iv = tc->iv;
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_ofb();
            break;
        case 192:
            cipher = EVP_aes_192_ofb();
            break;
        case 256:
            cipher = EVP_aes_256_ofb();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_CBC:
        iv = tc->iv;
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_cbc();
            break;
        case 192:
            cipher = EVP_aes_192_cbc();
            break;
        case 256:
            cipher = EVP_aes_256_cbc();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        break;
    case AMVP_SUB_AES_CBC_CS1:
    case AMVP_SUB_AES_CBC_CS2:
    case AMVP_SUB_AES_CBC_CS3:
        printf("AES-CBC-CSX algorithms are unsupported currently\n");
        rv = 1;
        goto err;
    case AMVP_SUB_AES_XTS:
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_xts();
            break;
        case 256:
            cipher = EVP_aes_256_xts();
            break;
        default:
            printf("Unsupported AES key length\n");
            rv = 1;
            goto err;
        }
        switch (tc->tw_mode) {
        case AMVP_SYM_CIPH_TWEAK_HEX:
            iv = tc->iv;
            break;
        case AMVP_SYM_CIPH_TWEAK_NUM:
        case AMVP_SYM_CIPH_TWEAK_NONE:
        default:
            printf("\nUnsupported tweak mode %d %d\n", tc->seq_num, tc->tw_mode);
            rv = 1;
            goto err;
            break;
        }
        break;
    case AMVP_SUB_AES_GCM:
    case AMVP_SUB_AES_GCM_SIV:
    case AMVP_SUB_AES_CCM:
    case AMVP_SUB_AES_XPN:
    case AMVP_SUB_AES_KW:
    case AMVP_SUB_AES_KWP:
    case AMVP_SUB_AES_GMAC:
    default:
        printf("Error: Unsupported AES mode requested by AMVP server\n");
        rv = 1;
        goto err;
    }

    /* If Monte Carlo we need to be able to init and then update
     * one thousand times before we complete each iteration.
     */
    if (tc->test_type == AMVP_SYM_TEST_TYPE_MCT) {
        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            if (tc->mct_index == 0) {
                EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, iv, 1);
                EVP_CIPHER_CTX_set_padding(cipher_ctx, 0);
                if (tc->cipher == AMVP_AES_CFB1) {
                    EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPH_FLAG_LENGTH_BITS);
                }
            }
            EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
            tc->ct_len = tc->pt_len;
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            if (tc->mct_index == 0) {
                EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, iv, 0);
                EVP_CIPHER_CTX_set_padding(cipher_ctx, 0);
                if (tc->cipher == AMVP_AES_CFB1) {
                    EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPH_FLAG_LENGTH_BITS);
                }
            }
            EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
            tc->pt_len = tc->ct_len;
        } else {
            printf("Unsupported direction\n");
            rv = 1;
            goto err;
        }
        if (tc->mct_index == AMVP_AES_MCT_INNER - 1) {
            EVP_CIPHER_CTX_free(cipher_ctx);
            glb_cipher_ctx = NULL;
        }
    } else {
        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, iv, 1);
            EVP_CIPHER_CTX_set_padding(cipher_ctx, 0);
            if (tc->cipher == AMVP_AES_CFB1) {
                EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPH_FLAG_LENGTH_BITS);
            }
            EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
            tc->ct_len = tc->pt_len;
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, iv, 0);
            EVP_CIPHER_CTX_set_padding(cipher_ctx, 0);
            if (tc->cipher == AMVP_AES_CFB1) {
                EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPH_FLAG_LENGTH_BITS);
            }
            EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
            tc->pt_len = tc->ct_len;
        } else {
            printf("Unsupported direction\n");
            rv = 1;
            goto err;
        }
        EVP_CIPHER_CTX_free(cipher_ctx);
        glb_cipher_ctx = NULL;
    }
    return rv;
err:
    if (glb_cipher_ctx) EVP_CIPHER_CTX_free(glb_cipher_ctx);
    glb_cipher_ctx = NULL;
    return rv;
}

/* NOTE - openssl does not support inverse option */
int app_aes_keywrap_handler(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC      *tc;
    EVP_CIPHER_CTX *cipher_ctx = NULL;
    const EVP_CIPHER        *cipher;
    int c_len;
    int rc = 1;
    AMVP_SUB_AES alg;

    if (!test_case) {
        return rc;
    }

    tc = test_case->tc.symmetric;

    if (tc->kwcipher != AMVP_SYM_KW_CIPHER) {
        printf("Invalid cipher for AES keywrap operation\n");
        return rc;
    }

    /* Begin encrypt code section */
    cipher_ctx = EVP_CIPHER_CTX_new();
    if (!cipher_ctx) {
        printf("Error creating CTX in AES keywrap\n");
        goto end;
    }
    EVP_CIPHER_CTX_init(cipher_ctx);

    alg = amvp_get_aes_alg(tc->cipher);
    if (alg == 0) {
        printf("Invalid cipher value");
        goto end;
    }

    switch (alg) {
    case AMVP_SUB_AES_KW:
    case AMVP_SUB_AES_KWP:
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_wrap();
            break;
        case 192:
            cipher = EVP_aes_192_wrap();
            break;
        case 256:
            cipher = EVP_aes_256_wrap();
            break;
        default:
            printf("Unsupported AES keywrap key length\n");
            goto end;
        }
        break;
    case AMVP_SUB_AES_GCM:
    case AMVP_SUB_AES_GCM_SIV:
    case AMVP_SUB_AES_CCM:
    case AMVP_SUB_AES_ECB:
    case AMVP_SUB_AES_CBC:
    case AMVP_SUB_AES_CBC_CS1:
    case AMVP_SUB_AES_CBC_CS2:
    case AMVP_SUB_AES_CBC_CS3:
    case AMVP_SUB_AES_CFB1:
    case AMVP_SUB_AES_CFB8:
    case AMVP_SUB_AES_CFB128:
    case AMVP_SUB_AES_OFB:
    case AMVP_SUB_AES_CTR:
    case AMVP_SUB_AES_XTS:
    case AMVP_SUB_AES_XPN:
    case AMVP_SUB_AES_GMAC:
    default:
        printf("Error: Unsupported AES keywrap mode requested by AMVP server\n");
        goto end;
    }

    if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
        EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);
        EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, NULL, 1);
        c_len = EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
        if (c_len <= 0) {
            goto end;
        } else {
            tc->ct_len = c_len;
        }
    } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
        EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);
        EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, NULL, 0);

#ifdef OPENSSL_KWP
        if (tc->cipher == AMVP_AES_KWP) {
            EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPHER_CTX_FLAG_UNWRAP_WITHPAD);
        }
#endif
        c_len = EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
        if (c_len <= 0) {
            goto end;
        } else {
            tc->pt_len = c_len;
        }
    } else {
        printf("Unsupported direction\n");
        goto end;
    }
    rc = 0;

end:
    /* Cleanup */
    if (cipher_ctx) EVP_CIPHER_CTX_free(cipher_ctx);

    return rc;
}

/*
 * This fuction is invoked by libamvp when an AES crypto
 * operation is needed from the crypto module being
 * validated.  This is a callback provided to libamvp when
 * amvp_enable_capability() is invoked to register the
 * AES-GCM capabilitiy with libamvp.  libamvp will in turn
 * invoke this function when it needs to process an AES-GCM
 * test case.
 */
int app_aes_handler_aead(AMVP_TEST_CASE *test_case) {
    AMVP_SYM_CIPHER_TC      *tc;
    EVP_CIPHER_CTX *cipher_ctx = NULL;
    const EVP_CIPHER        *cipher;
    unsigned char iv_fixed[4] = { 1, 2, 3, 4 };
    int rc = 0;
    int ret = 0;
    AMVP_SUB_AES alg;

    if (!test_case) {
        return 1;
    }

    tc = test_case->tc.symmetric;

    if (tc->direction != AMVP_SYM_CIPH_DIR_ENCRYPT && tc->direction != AMVP_SYM_CIPH_DIR_DECRYPT) {
        printf("Unsupported direction\n");
        return 1;
    }

    /* Begin encrypt code section */
    cipher_ctx = EVP_CIPHER_CTX_new();
    if (!cipher_ctx) {
        printf("Error initializing cipher CTX\n");
        rc = 1;
        goto end;
    }
    EVP_CIPHER_CTX_init(cipher_ctx);

    /* Validate key length and assign OpenSSL EVP cipher */
    alg = amvp_get_aes_alg(tc->cipher);
    if (alg == 0) {
        printf("Invalid cipher value");
        goto end;
    }

    switch (alg) {
    case AMVP_SUB_AES_GMAC:
    case AMVP_SUB_AES_GCM:
        if (tc->cipher == AMVP_AES_GMAC && (tc->pt_len || tc->ct_len)) {
            printf("Invalid AES-GMAC ct/pt data\n");
            rc = 1;
            goto end;
        }
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_gcm();
            break;
        case 192:
            cipher = EVP_aes_192_gcm();
            break;
        case 256:
            cipher = EVP_aes_256_gcm();
            break;
        default:
            printf("Unsupported AES-GCM key length\n");
            rc = 1;
            goto end;
        }
        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPH_FLAG_NON_FIPS_ALLOW);
            EVP_CipherInit(cipher_ctx, cipher, NULL, NULL, 1);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IVLEN, tc->iv_len, 0);
            EVP_CipherInit(cipher_ctx, NULL, tc->key, NULL, 1);

            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IV_FIXED, 4, iv_fixed);
            if (!EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_IV_GEN, tc->iv_len, tc->iv)) {
                printf("amvp_aes_encrypt: iv gen error\n");
                rc = 1;
                goto end;
            }
            if (tc->aad_len) {
                EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            }
            if (tc->cipher != AMVP_AES_GMAC) {
                EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
            }
            EVP_Cipher(cipher_ctx, NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_GET_TAG, tc->tag_len, tc->tag);
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            EVP_CIPHER_CTX_set_flags(cipher_ctx, EVP_CIPH_FLAG_NON_FIPS_ALLOW);
            EVP_CipherInit_ex(cipher_ctx, cipher, NULL, tc->key, NULL, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IVLEN, tc->iv_len, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_IV_FIXED, -1, tc->iv);
            if (!EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_IV_GEN, tc->iv_len, tc->iv)) {
                printf("\nFailed to set IV");
                rc = 1;
                goto end;
            }
            if (tc->aad_len) {
                /*
                 * Set dummy tag before processing AAD.  Otherwise the AAD can
                 * not be processed.
                 */
                EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_TAG, tc->tag_len, tc->tag);
                EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            }
            /*
             * Set the tag when decrypting
             */
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_GCM_SET_TAG, tc->tag_len, tc->tag);

            /*
             * Decrypt the CT
             */
            if (tc->cipher != AMVP_AES_GMAC) {
                EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
            }
            /*
             * Check the tag
             */
            ret = EVP_Cipher(cipher_ctx, NULL, NULL, 0);
            if (ret) {
                rc = 1;
                goto end;
            }
        }
        break;
    case AMVP_SUB_AES_CCM:
        switch (tc->key_len) {
        case 128:
            cipher = EVP_aes_128_ccm();
            break;
        case 192:
            cipher = EVP_aes_192_ccm();
            break;
        case 256:
            cipher = EVP_aes_256_ccm();
            break;
        default:
            printf("Unsupported AES-CCM key length\n");
            rc = 1;
            goto end;
        }
        if (tc->direction == AMVP_SYM_CIPH_DIR_ENCRYPT) {
            EVP_CipherInit(cipher_ctx, cipher, NULL, NULL, 1);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_IVLEN, tc->iv_len, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_TAG, tc->tag_len, 0);
            EVP_CipherInit(cipher_ctx, NULL, tc->key, tc->iv, 1);
            EVP_Cipher(cipher_ctx, NULL, NULL, tc->pt_len);
            EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            ret = EVP_Cipher(cipher_ctx, tc->ct, tc->pt, tc->pt_len);
            if (ret < 0) {
                printf("Error performing encrypt operation CCM\n");
                rc = 1;
                goto end;
            }
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_GET_TAG, tc->tag_len, tc->ct + tc->ct_len);
            tc->ct_len += tc->tag_len;
        } else if (tc->direction == AMVP_SYM_CIPH_DIR_DECRYPT) {
            EVP_CipherInit(cipher_ctx, cipher, NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_IVLEN, tc->iv_len, 0);
            EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_CCM_SET_TAG, tc->tag_len, tc->ct + tc->pt_len);
            EVP_CipherInit(cipher_ctx, NULL, tc->key, tc->iv, 0);
            EVP_Cipher(cipher_ctx, NULL, NULL, tc->pt_len);
            EVP_Cipher(cipher_ctx, NULL, tc->aad, tc->aad_len);
            /*
             * Decrypt and check the tag
             */
            ret = EVP_Cipher(cipher_ctx, tc->pt, tc->ct, tc->ct_len);
            if (ret < 0) {
                rc = 1;
                goto end;
            }
        }
        break;
    case AMVP_SUB_AES_GCM_SIV:
    case AMVP_SUB_AES_ECB:
    case AMVP_SUB_AES_CBC:
    case AMVP_SUB_AES_CFB1:
    case AMVP_SUB_AES_CFB8:
    case AMVP_SUB_AES_CFB128:
    case AMVP_SUB_AES_OFB:
    case AMVP_SUB_AES_CTR:
    case AMVP_SUB_AES_XTS:
    case AMVP_SUB_AES_KW:
    case AMVP_SUB_AES_KWP:
    case AMVP_SUB_AES_XPN:
    case AMVP_SUB_AES_CBC_CS1:
    case AMVP_SUB_AES_CBC_CS2:
    case AMVP_SUB_AES_CBC_CS3:
    default:
        printf("Error: Unsupported AES AEAD mode requested by AMVP server\n");
        rc = 1;
        goto end;
    }

end:
    /* Cleanup */
    if (cipher_ctx) EVP_CIPHER_CTX_free(cipher_ctx);

    return rc;
}

#endif
