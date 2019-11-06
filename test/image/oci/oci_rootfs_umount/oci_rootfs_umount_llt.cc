/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * iSulad licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 * Description: oci_rootfs_umount llt
 * Author: wangfengtu
 * Create: 2019-08-26
 */

#include <stdlib.h>
#include <stdio.h>
#include <securec.h>
#include <gtest/gtest.h>
#include "mock.h"
#include "utils.h"
#include "oci_rootfs_umount.h"
#include "oci_llt_common.h"

static int g_exec_cmd_count = 0;

DECLARE_OCI_LLT_COMMON_WRAPPER

extern "C" {
    DECLARE_WRAPPER_V(util_exec_cmd, bool, \
                      (exec_func_t cb_func, void *args, const char *stdin_msg, char **stdout_msg, char **stderr_msg));
    DEFINE_WRAPPER_V(util_exec_cmd, bool, \
                     (exec_func_t cb_func, void *args, const char *stdin_msg, char **stdout_msg, char **stderr_msg), \
                     (cb_func, args, stdin_msg, stdout_msg, stderr_msg));
}

static bool util_exec_cmd_fail(exec_func_t cb_func, void *args, const char *stdin_msg, char **stdout_msg,
                               char **stderr_msg)
{
    g_exec_cmd_count++;
    if (g_exec_cmd_count == 1) {
        *stderr_msg = util_strdup_s("Device or Resource Busy");
        return false;
    }

    if (g_exec_cmd_count == 2) {
        *stderr_msg = NULL;
        return false;
    }

    return 0;
}


TEST(oci_rootfs_umount_llt, test_umount_rootfs)
{
    rootfs_umount_request *req = NULL;
    rootfs_umount_response *resp = NULL;
    int i = 0;

    // Test parameter NULL
    ASSERT_NE(umount_rootfs(NULL, NULL), 0);
    ASSERT_NE(umount_rootfs(NULL, &resp), 0);
    free_rootfs_umount_response(resp);
    resp = NULL;

    // Test content of parameter NULL
    req = (rootfs_umount_request*)util_common_calloc_s(sizeof(rootfs_umount_request));
    ASSERT_TRUE(req != NULL);
    ASSERT_NE(umount_rootfs(req, &resp), 0);
    free_rootfs_umount_request(req);
    req = NULL;
    free_rootfs_umount_response(resp);
    resp = NULL;

    // Test parameter collect
    req = (rootfs_umount_request*)util_common_calloc_s(sizeof(rootfs_umount_request));
    ASSERT_TRUE(req != NULL);
    req->name_id = util_strdup_s("image_name");
    ASSERT_TRUE(req->name_id != NULL);

    MOCK_SET_DEFAULT_ISULAD_KIT_OPTS
    MOCK_SET_V(execvp, execvp_success);
    ASSERT_EQ(umount_rootfs(req, &resp), 0);
    ASSERT_TRUE(resp != NULL);
    MOCK_CLEAR_DEFAULT_ISULAD_KIT_OPTS
    MOCK_CLEAR(execvp);

    free_rootfs_umount_request(req);
    req = NULL;
    free_rootfs_umount_response(resp);
    resp = NULL;

    // Put MOCK_SET_V here to avoid warning "(style) Local variable MOCK_SET_V shadows outer variable"
    MOCK_SET_V(util_exec_cmd, util_exec_cmd_fail);
    // 1. Test util_exec_cmd failed with stderr_buffer != NULL
    // 2. Test util_exec_cmd failed with stderr_buffer == NULL
    for (i = 0; i < 2; i++) {
        req = (rootfs_umount_request*)util_common_calloc_s(sizeof(rootfs_umount_request));
        ASSERT_TRUE(req != NULL);
        req->name_id = util_strdup_s("image_name");
        ASSERT_TRUE(req->name_id != NULL);

        MOCK_SET_DEFAULT_ISULAD_KIT_OPTS
        ASSERT_NE(umount_rootfs(req, &resp), 0);
        MOCK_CLEAR_DEFAULT_ISULAD_KIT_OPTS

        free_rootfs_umount_request(req);
        req = NULL;
        free_rootfs_umount_response(resp);
        resp = NULL;
    }
    MOCK_CLEAR(util_exec_cmd);
}

TEST(oci_rootfs_umount_llt, test_free_rootfs_umount_request)
{
    rootfs_umount_request *req = NULL;

    // Test free NULL
    free_rootfs_umount_request(NULL);

    // Test free name_id NULL
    req = (rootfs_umount_request*)util_common_calloc_s(sizeof(rootfs_umount_request));
    ASSERT_TRUE(req != NULL);

    free_rootfs_umount_request(req);
    req = NULL;

    // Test free all Not NULL
    req = (rootfs_umount_request*)util_common_calloc_s(sizeof(rootfs_umount_request));
    ASSERT_TRUE(req != NULL);
    req->name_id = util_strdup_s("image_name");
    ASSERT_TRUE(req->name_id != NULL);

    free_rootfs_umount_request(req);
    req = NULL;
}

TEST(oci_rootfs_umount_llt, test_free_rootfs_umount_response)
{
    rootfs_umount_response *resp = NULL;

    // Test free NULL
    free_rootfs_umount_response(NULL);

    // Test free errmsg NULL
    resp = (rootfs_umount_response*)util_common_calloc_s(sizeof(rootfs_umount_response));
    ASSERT_TRUE(resp != NULL);

    free_rootfs_umount_response(resp);
    resp = NULL;

    // Test free all Not NULL
    resp = (rootfs_umount_response*)util_common_calloc_s(sizeof(rootfs_umount_response));
    ASSERT_TRUE(resp != NULL);
    resp->errmsg = util_strdup_s("This is error message");
    ASSERT_TRUE(resp->errmsg != NULL);

    free_rootfs_umount_response(resp);
    resp = NULL;
}
