/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * iSulad licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 * Author: tanyifeng
 * Create: 2018-11-08
 * Description: provide container lcrc library definition
 ******************************************************************************/
#ifndef __LIB_LCRC_H
#define __LIB_LCRC_H

#include <stdbool.h>
#include <stdint.h>

#include "container_def.h"
#include "container_path_stat.h"
#include "json_common.h"
#include "console.h"

#ifdef __cplusplus
extern "C" {
#endif

struct lcrc_filters {
    char **keys;
    char **values;
    size_t len;
};

typedef struct lcrc_container_config {
    char **env;
    size_t env_len;

    char *hostname;

    char *user;

    bool attach_stdin;

    bool attach_stdout;

    bool attach_stderr;

    bool open_stdin;

    bool tty;

    bool readonly;

    bool all_devices;

    bool system_container;
    char *ns_change_opt;

    char **mounts;
    size_t mounts_len;

    char *entrypoint;

    char **cmd;
    size_t cmd_len;

    char *log_file;

    char *log_file_size;

    unsigned int log_file_rotate;

    json_map_string_string *annotations;

    char *workdir;

    char *health_cmd;

    int64_t health_interval;

    int health_retries;

    int64_t health_timeout;

    int64_t health_start_period;

    bool no_healthcheck;

    bool exit_on_unhealthy;

    char **accel;
    size_t accel_len;
} lcrc_container_config_t;

typedef struct lcrc_host_config {
    char **devices;
    size_t devices_len;

    char **hugetlbs;
    size_t hugetlbs_len;

    char **group_add;
    size_t group_add_len;

    char *network_mode;

    char *ipc_mode;

    char *pid_mode;

    char *uts_mode;

    char *userns_mode;

    char *user_remap;

    char **ulimits;
    size_t ulimits_len;

    char *restart_policy;

    char *host_channel;

    char **cap_add;
    size_t cap_add_len;

    char **cap_drop;
    size_t cap_drop_len;

    json_map_string_string *storage_opts;

    json_map_string_string *sysctls;

    char **dns;
    size_t dns_len;

    char **dns_options;
    size_t dns_options_len;

    char **dns_search;
    size_t dns_search_len;

    char **extra_hosts;
    size_t extra_hosts_len;

    char *hook_spec;

    char **binds;
    size_t binds_len;

    char **blkio_weight_device;
    size_t blkio_weight_device_len;

    char **blkio_throttle_read_bps_device;
    size_t blkio_throttle_read_bps_device_len;

    char **blkio_throttle_write_bps_device;
    size_t blkio_throttle_write_bps_device_len;

    bool privileged;
    bool system_container;
    char **ns_change_files;
    size_t ns_change_files_len;
    bool auto_remove;

    bool oom_kill_disable;

    int64_t shm_size;

    bool readonly_rootfs;

    char *env_target_file;

    char *cgroup_parent;

    container_cgroup_resources_t *cr;

    char **security;
    size_t security_len;
} lcrc_host_config_t;

struct lcrc_create_request {
    char *name;
    char *rootfs;
    char *image;
    char *runtime;
    lcrc_host_config_t *hostconfig;
    lcrc_container_config_t *config;
};

struct lcrc_create_response {
    char *id;
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_start_request {
    char *name;
    char *stdin;
    bool attach_stdin;
    char *stdout;
    bool attach_stdout;
    char *stderr;
    bool attach_stderr;
};

struct lcrc_start_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_top_request {
    char *name;
    int ps_argc;
    char **ps_args;
};

struct lcrc_top_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
    char *titles;
    char **processes;
    size_t processes_len;
};

struct lcrc_stop_request {
    char *name;
    bool force;
    int timeout;
};

struct lcrc_stop_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_restart_request {
    char *name;
    unsigned int timeout;
};

struct lcrc_restart_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_kill_request {
    char *name;
    uint32_t signal;
};

struct lcrc_kill_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_delete_request {
    char *name;
    bool force;
};

struct lcrc_delete_response {
    char *name;
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_pause_request {
    char *name;
};

struct lcrc_pause_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_resume_request {
    char *name;
};

struct lcrc_resume_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_container_info {
    char *id;
    bool has_pid;
    uint32_t pid;
    Container_Status status;
    uint64_t pids_current;
    // CPU usage
    uint64_t cpu_use_nanos;
    uint64_t cpu_system_use;
    uint32_t online_cpus;
    // BlkIO usage
    uint64_t blkio_read;
    uint64_t blkio_write;
    // Memory usage
    uint64_t mem_used;
    uint64_t mem_limit;
    // Kernel Memory usage
    uint64_t kmem_used;
    uint64_t kmem_limit;
};

struct lcrc_inspect_request {
    char *name;
    bool bformat;
    int timeout;
};

struct lcrc_inspect_response {
    uint32_t cc;
    uint32_t server_errono;
    char *json;
    char *errmsg;
};

struct lcrc_list_request {
    struct lcrc_filters *filters;
    bool all;
};

struct lcrc_container_summary_info {
    char *id;
    char *name;
    uint32_t has_pid;
    uint32_t pid;
    Container_Status status;
    char *image;
    char *command;
    uint32_t exit_code;
    uint32_t restart_count;
    char *startat;
    char *finishat;
    char *runtime;
    char *health_state;
};

struct lcrc_list_response {
    uint32_t cc;
    uint32_t server_errono;
    size_t container_num;
    struct lcrc_container_summary_info **container_summary;
    char *errmsg;
};

struct lcrc_stats_request {
    char *runtime;
    char **containers;
    size_t containers_len;
    bool all;
};

struct lcrc_stats_response {
    uint32_t cc;
    uint32_t server_errono;
    size_t container_num;
    struct lcrc_container_info *container_stats;
    char *errmsg;
};

struct lcrc_events_request {
    container_events_callback_t cb;
    bool storeonly;
    char *id;
    types_timestamp_t since;
    types_timestamp_t until;
};

struct lcrc_events_response {
    uint32_t server_errono;
    uint32_t cc;
    char *errmsg;
};

struct lcrc_copy_from_container_request {
    char *id;
    char *runtime;
    char *srcpath;
};

struct lcrc_copy_from_container_response {
    uint32_t server_errono;
    uint32_t cc;
    char *errmsg;
    container_path_stat *stat;
    struct io_read_wrapper reader;
};

struct lcrc_copy_to_container_request {
    char *id;
    char *runtime;
    char *srcpath;
    char *srcrebase;
    bool srcisdir;
    char *dstpath;
    struct io_read_wrapper reader;
};

struct lcrc_copy_to_container_response {
    uint32_t server_errono;
    uint32_t cc;
    char *errmsg;
};

struct lcrc_logs_request {
    char *id;
    char *runtime;

    char *since;
    char *until;
    bool timestamps;
    bool follow;
    int64_t tail;
    bool details;
};

struct lcrc_logs_response {
    uint32_t server_errono;
    uint32_t cc;
    char *errmsg;
};

struct lcrc_wait_request {
    char *id;
    uint32_t condition;
};

struct lcrc_wait_response {
    int exit_code;
    uint32_t server_errono;
    uint32_t cc;
    char *errmsg;
};

struct lcrc_exec_request {
    char *name;
    bool tty;
    bool open_stdin;
    bool attach_stdin;
    bool attach_stdout;
    bool attach_stderr;
    char *stdin;
    char *stdout;
    char *stderr;
    int argc;
    char **argv;
    size_t env_len;
    char **env;
    int64_t timeout;
};

struct lcrc_exec_response {
    uint32_t cc;
    uint32_t server_errono;
    uint32_t pid;
    uint32_t exit_code;
    char *errmsg;
};

struct lcrc_attach_request {
    char *name;
    char *stdin;
    char *stdout;
    char *stderr;
    bool attach_stdin;
    bool attach_stdout;
    bool attach_stderr;
};

struct lcrc_attach_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_health_check_request {
    char *service;
};

struct lcrc_health_check_response {
    Health_Serving_Status health_status;
    uint32_t cc;
    char *errmsg;
};

struct lcrc_version_request {
    char unuseful;
};

struct lcrc_version_response {
    uint32_t cc;
    uint32_t server_errono;
    char *version;
    char *git_commit;
    char *build_time;
    char *root_path;
    char *errmsg;
};

struct lcrc_info_request {
    char unuseful;
};

struct lcrc_info_response {
    uint32_t cc;
    uint32_t server_errono;
    char *version;
    char *kversion;
    char *os_type;
    char *architecture;
    char *nodename;
    char *operating_system;
    char *cgroup_driver;
    char *logging_driver;
    char *huge_page_size;
    char *isulad_root_dir;
    char *http_proxy;
    char *https_proxy;
    char *no_proxy;
    uint32_t total_mem;
    uint32_t containers_num;
    uint32_t c_running;
    uint32_t c_paused;
    uint32_t c_stopped;
    uint32_t images_num;
    uint32_t cpus;
    char *errmsg;
};

struct lcrc_container_conf_request {
    char *name;
};

struct lcrc_container_conf_response {
    uint32_t cc;
    uint32_t server_errono;
    char *container_logpath;
    uint32_t container_logrotate;
    char *container_logsize;
    char *errmsg;
};

typedef struct lcrc_update_config {
    char *restart_policy;
    container_cgroup_resources_t *cr;
} lcrc_update_config_t;

struct lcrc_update_request {
    char *name;
    lcrc_update_config_t *updateconfig;
};

struct lcrc_update_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_image_info {
    char *imageref;
    char *type;
    char *digest;
    int64_t created; /* seconds */
    int32_t created_nanos;
    int64_t size; /*Bytes*/
};

struct lcrc_create_image_request {
    struct lcrc_image_info image_info;
};

struct lcrc_create_image_response {
    uint32_t cc;
    uint32_t server_errono;
    struct lcrc_image_info image_info;
    char *errmsg;
};

struct lcrc_list_images_request {
    // unuseful definition to avoid generate empty struct which will get 0 if we call sizeof
    char unuseful;
};

struct lcrc_list_images_response {
    uint32_t cc;
    uint32_t server_errono;
    size_t images_num;
    struct lcrc_image_info *images_list;
    char *errmsg;
};

struct lcrc_rmi_request {
    char *image_name;
    bool force;
};

struct lcrc_rmi_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_pull_request {
    char *image_name;
};

struct lcrc_pull_response {
    char *image_ref;
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_load_request {
    char *socketname;
    char *file;
    char *type;
    char *tag;
};

struct lcrc_load_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_login_request {
    char *socketname;
    char *username;
    char *password;
    char *server;
    char *type;
};

struct lcrc_login_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_logout_request {
    char *socketname;
    char *server;
    char *type;
};

struct lcrc_logout_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_export_request {
    char *name;
    char *file;
};

struct lcrc_export_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

struct lcrc_rename_request {
    char *old_name;
    char *new_name;
};

struct lcrc_rename_response {
    uint32_t cc;
    uint32_t server_errono;
    char *errmsg;
};

Container_Status lcrcstastr2sta(const char *state);

struct lcrc_filters *lcrc_filters_parse_args(const char **array, size_t len);

void lcrc_filters_free(struct lcrc_filters *filters);

void lcrc_container_info_free(struct lcrc_container_info *info);

void lcrc_version_request_free(struct lcrc_version_request *request);

void lcrc_version_response_free(struct lcrc_version_response *response);

void lcrc_info_request_free(struct lcrc_info_request *request);

void lcrc_info_response_free(struct lcrc_info_response *response);

void lcrc_ns_change_files_free(lcrc_host_config_t *hostconfig);

void lcrc_host_config_storage_opts_free(lcrc_host_config_t *hostconfig);

void lcrc_host_config_sysctl_free(lcrc_host_config_t *hostconfig);

void lcrc_host_config_free(lcrc_host_config_t *hostconfig);

void lcrc_container_config_free(lcrc_container_config_t *config);

void lcrc_create_request_free(struct lcrc_create_request *request);

void lcrc_create_response_free(struct lcrc_create_response *response);

void lcrc_start_request_free(struct lcrc_start_request *request);

void lcrc_start_response_free(struct lcrc_start_response *response);

void lcrc_top_request_free(struct lcrc_top_request *request);

void lcrc_top_response_free(struct lcrc_top_response *response);

void lcrc_stop_request_free(struct lcrc_stop_request *request);

void lcrc_stop_response_free(struct lcrc_stop_response *response);

void lcrc_restart_request_free(struct lcrc_restart_request *request);

void lcrc_restart_response_free(struct lcrc_restart_response *response);

void lcrc_delete_request_free(struct lcrc_delete_request *request);

void lcrc_delete_response_free(struct lcrc_delete_response *response);

void lcrc_list_request_free(struct lcrc_list_request *request);

void lcrc_list_response_free(struct lcrc_list_response *response);

void lcrc_exec_request_free(struct lcrc_exec_request *request);

void lcrc_exec_response_free(struct lcrc_exec_response *response);

void lcrc_attach_request_free(struct lcrc_attach_request *request);

void lcrc_attach_response_free(struct lcrc_attach_response *response);

void lcrc_pause_request_free(struct lcrc_pause_request *request);

void lcrc_pause_response_free(struct lcrc_pause_response *response);

void lcrc_resume_request_free(struct lcrc_resume_request *request);

void lcrc_resume_response_free(struct lcrc_resume_response *response);

void lcrc_container_conf_request_free(struct lcrc_container_conf_request *req);

void lcrc_container_conf_response_free(struct lcrc_container_conf_response *resp);

void lcrc_kill_request_free(struct lcrc_kill_request *request);

void lcrc_kill_response_free(struct lcrc_kill_response *response);

void lcrc_update_config_free(lcrc_update_config_t *config);

void lcrc_update_request_free(struct lcrc_update_request *request);

void lcrc_update_response_free(struct lcrc_update_response *response);

void lcrc_stats_request_free(struct lcrc_stats_request *request);

void lcrc_stats_response_free(struct lcrc_stats_response *response);

void lcrc_events_request_free(struct lcrc_events_request *request);

void lcrc_events_response_free(struct lcrc_events_response *response);

void lcrc_copy_from_container_request_free(struct lcrc_copy_from_container_request *request);

void lcrc_copy_from_container_response_free(struct lcrc_copy_from_container_response *response);

void lcrc_copy_to_container_request_free(struct lcrc_copy_to_container_request *request);

void lcrc_copy_to_container_response_free(struct lcrc_copy_to_container_response *response);

void lcrc_inspect_request_free(struct lcrc_inspect_request *request);

void lcrc_inspect_response_free(struct lcrc_inspect_response *response);

void lcrc_wait_request_free(struct lcrc_wait_request *request);

void lcrc_wait_response_free(struct lcrc_wait_response *response);

void lcrc_health_check_request_free(struct lcrc_health_check_request *request);

void lcrc_health_check_response_free(struct lcrc_health_check_response *response);

void lcrc_create_image_request_free(struct lcrc_create_image_request *request);

void lcrc_create_image_response_free(struct lcrc_create_image_response *response);

void lcrc_images_list_free(size_t images_num, struct lcrc_image_info *images_list);

void lcrc_list_images_request_free(struct lcrc_list_images_request *request);

void lcrc_list_images_response_free(struct lcrc_list_images_response *response);

void lcrc_rmi_request_free(struct lcrc_rmi_request *request);

void lcrc_rmi_response_free(struct lcrc_rmi_response *response);

void lcrc_load_request_free(struct lcrc_load_request *request);

void lcrc_load_response_free(struct lcrc_load_response *response);

void lcrc_login_response_free(struct lcrc_login_response *response);

void lcrc_logout_response_free(struct lcrc_logout_response *response);

void lcrc_pull_request_free(struct lcrc_pull_request *request);
void lcrc_pull_response_free(struct lcrc_pull_response *response);

void lcrc_export_request_free(struct lcrc_export_request *request);

void lcrc_export_response_free(struct lcrc_export_response *response);

void lcrc_rename_request_free(struct lcrc_rename_request *request);

void lcrc_rename_response_free(struct lcrc_rename_response *response);

void lcrc_logs_request_free(struct lcrc_logs_request *request);
void lcrc_logs_response_free(struct lcrc_logs_response *response);

#ifdef __cplusplus
}
#endif

#endif

