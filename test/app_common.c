/** @file */
/*
 * Copyright (c) 2021, Cisco Systems, Inc.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/cisco/libamvp/LICENSE
 */


#include "app_common.h"
#include "ut_common.h"
#include <openssl/hmac.h>


AMVP_RESULT totp(char **token, int token_max);


/* Here just to avoid warning */

void dummy_call(void)
{

}
