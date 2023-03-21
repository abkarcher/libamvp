/** @file */
/*
 * Copyright (c) 2019, Cisco Systems, Inc.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/cisco/libamvp/LICENSE
 */


#include <string.h>
#include <stdio.h>
#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "amvp/parson.h"
#include "safe_lib.h"
#include "amvp/amvp.h"

int counter_set;
int counter_fail;

void teardown_ctx(AMVP_CTX **ctx);
AMVP_RESULT progress(char *msg, AMVP_LOG_LVL level);
void setup_empty_ctx(AMVP_CTX **ctx);
int dummy_handler_success(AMVP_TEST_CASE *test_case);
int dummy_handler_failure(AMVP_TEST_CASE *test_case);
JSON_Object *ut_get_obj_from_rsp (JSON_Value *arry_val);
unsigned int base64_decode(const char *in, unsigned int inlen, unsigned char *out);
unsigned int dummy_totp(char **token, int token_max);
