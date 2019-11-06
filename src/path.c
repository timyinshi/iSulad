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
 * Description: provide container path functions
 ******************************************************************************/
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <libgen.h>
#include <sys/stat.h>

#include "log.h"
#include "path.h"
#include "utils.h"
#include "securec.h"

#define ISSLASH(C) ((C) == '/')
#define IS_ABSOLUTE_FILE_NAME(F) (ISSLASH ((F)[0]))
#define IS_RELATIVE_FILE_NAME(F) (!IS_ABSOLUTE_FILE_NAME (F))

static bool do_clean_path_continue(const char *endpos, const char *stpos, const char *respath, char **dst)
{
    if (endpos - stpos == 1 && stpos[0] == '.') {
        return true;
    } else if (endpos - stpos == 2 && stpos[0] == '.' && stpos[1] == '.') {
        char *dest = *dst;
        if (dest <= respath + 1) {
            return true;
        }
        for (--dest; dest > respath && !ISSLASH(dest[-1]); --dest) {
            continue;
        }
        *dst = dest;
        return true;
    }
    return false;
}

static int do_clean_path(const char *respath, const char *limit_respath,
                         const char *stpos, char **dst)
{
    char *dest = *dst;
    const char *endpos = NULL;
    errno_t ret;

    for (endpos = stpos; *stpos; stpos = endpos) {
        while (ISSLASH(*stpos)) {
            ++stpos;
        }

        for (endpos = stpos; *endpos && !ISSLASH(*endpos); ++endpos) {
        }

        if (endpos - stpos == 0) {
            break;
        } else if (do_clean_path_continue(endpos, stpos, respath, &dest)) {
            continue;
        }

        if (!ISSLASH(dest[-1])) {
            *dest++ = '/';
        }

        if (dest + (endpos - stpos) >= limit_respath) {
            ERROR("Path is too long");
            if (dest > respath + 1) {
                dest--;
            }
            *dest = '\0';
            return -1;
        }

        ret = memcpy_s(dest, (size_t)(endpos - stpos), stpos, (size_t)(endpos - stpos));
        if (ret != EOK) {
            ERROR("Failed at cleanpath memcpy");
            return -1;
        }
        dest += endpos - stpos;
        *dest = '\0';
    }
    *dst = dest;
    return 0;
}

char *cleanpath(const char *path, char *realpath, size_t realpath_len)
{
    char *respath = NULL;
    char *dest = NULL;
    const char *stpos = NULL;
    const char *limit_respath = NULL;
    errno_t ret;

    if (path == NULL || path[0] == '\0' || \
        realpath == NULL || (realpath_len < PATH_MAX)) {
        return NULL;
    }

    respath = realpath;

    ret = memset_s(respath, realpath_len, 0, realpath_len);
    if (ret != EOK) {
        ERROR("Failed at cleanpath memset");
        goto error;
    }
    limit_respath = respath + PATH_MAX;

    if (!IS_ABSOLUTE_FILE_NAME(path)) {
        if (!getcwd(respath, PATH_MAX)) {
            ERROR("Failed to getcwd");
            respath[0] = '\0';
            goto error;
        }
        dest = strchr(respath, '\0');
        if (dest == NULL) {
            ERROR("Failed to get the end of respath");
            goto error;
        }
        ret = strcat_s(respath, PATH_MAX, path);
        if (ret != EOK) {
            ERROR("Failed at cleanpath strcat");
            goto error;
        }
        stpos = path;
    } else {
        dest = respath;
        *dest++ = '/';
        stpos = path;
    }

    if (do_clean_path(respath, limit_respath, stpos, &dest)) {
        goto error;
    }

    if (dest > respath + 1 && ISSLASH(dest[-1])) {
        --dest;
    }
    *dest = '\0';

    return respath;

error:
    return NULL;
}

static int do_path_realloc(const char *start, const char *end,
                           char **rpath, char **dest, const char **rpath_limit)
{
    int nret = 0;
    size_t new_size;
    size_t gap = 0;
    long long dest_offset = *dest - *rpath;
    char *new_rpath = NULL;

    if (*dest + (end - start) < *rpath_limit) {
        return 0;
    }

    gap = (size_t)(end - start) + 1;
    new_size = (size_t)(*rpath_limit - *rpath);
    if (new_size > SIZE_MAX - gap) {
        ERROR("Out of range!");
        return -1;
    }

    if (gap > PATH_MAX) {
        new_size += gap;
    } else {
        new_size += PATH_MAX;
    }
    nret = mem_realloc((void **)(&new_rpath), new_size, *rpath, PATH_MAX);
    if (nret) {
        ERROR("Failed to realloc memory for files limit variables");
        return -1;
    }
    *rpath = new_rpath;
    *rpath_limit = *rpath + new_size;

    *dest = *rpath + dest_offset;

    return 0;
}

static int do_get_symlinks_copy_buf(const char *buf, const char *prefix, size_t prefix_len,
                                    char **rpath, char **dest)
{
    if (IS_ABSOLUTE_FILE_NAME(buf)) {
        if (prefix_len) {
            if (memcpy_s(*rpath, PATH_MAX, prefix, prefix_len) != EOK) {
                ERROR("Memory copy failed!");
                return -1;
            }
        }
        *dest = *rpath + prefix_len;
        *(*dest)++ = '/';
    } else {
        if (*dest > *rpath + prefix_len + 1) {
            for (--(*dest); *dest > *rpath && !ISSLASH((*dest)[-1]); --(*dest)) {
                continue;
            }
        }
    }
    return 0;
}

static int do_get_symlinks(const char **fullpath, const char *prefix, size_t prefix_len,
                           char **rpath, char **dest, const char **end,
                           int *num_links, char **extra_buf)
{
    int ret = -1;
    size_t len;
    ssize_t n;
    errno_t rc = EOK;
    char *buf = NULL;

    if (++(*num_links) > MAXSYMLINKS) {
        ERROR("Too many links in '%s'", *fullpath);
        goto out;
    }

    buf = util_common_calloc_s(PATH_MAX);
    if (buf == NULL) {
        ERROR("Out of memory");
        goto out;
    }

    n = readlink(*rpath, buf, PATH_MAX - 1);
    if (n < 0) {
        goto out;
    }
    buf[n] = '\0';

    if (*extra_buf == NULL) {
        *extra_buf = util_common_calloc_s(PATH_MAX);
        if (*extra_buf == NULL) {
            ERROR("Out of memory");
            goto out;
        }
    }

    len = strlen(*end);
    if (len >= PATH_MAX - (size_t)n) {
        ERROR("Path is too long");
        goto out;
    }

    rc = memmove_s(&(*extra_buf)[n], (size_t)(PATH_MAX - n), *end, len + 1);
    if (rc != EOK) {
        ERROR("Memory move failed!");
        goto out;
    }
    rc = memcpy_s(*extra_buf, PATH_MAX, buf, (size_t)n);
    if (rc != EOK) {
        ERROR("Memory copy failed!");
        goto out;
    }
    *fullpath = *end = *extra_buf;

    if (do_get_symlinks_copy_buf(buf, prefix, prefix_len, rpath, dest) != 0) {
        goto out;
    }

    ret = 0;
out:
    free(buf);
    return ret;
}

static bool do_eval_symlinks_in_scope_is_symlink(const char *path)
{
    struct stat st;

    if (lstat(path, &st) < 0) {
        return true;
    }

    if (!S_ISLNK(st.st_mode)) {
        return true;
    }
    return false;
}

static void do_eval_symlinks_skip_slash(const char **start, const char **end)
{
    while (ISSLASH(**start)) {
        ++(*start);
    }

    for (*end = *start; **end && !ISSLASH(**end); ++(*end)) {
    }
}

static inline void skip_dest_trailing_slash(char **dest, char **rpath, size_t prefix_len)
{
    if (*dest > *rpath + prefix_len + 1) {
        for (--(*dest); *dest > *rpath && !ISSLASH((*dest)[-1]); --(*dest)) {
            continue;
        }
    }
}

static inline bool is_current_char(const char c)
{
    return c == '.';
}

static inline bool is_specify_current(const char *end, const char *start)
{
    return (end - start == 1) && is_current_char(start[0]);
}

static inline bool is_specify_parent(const char *end, const char *start)
{
    return (end - start == 2) && is_current_char(start[0]) && is_current_char(start[1]);
}

static int do_eval_symlinks_in_scope(const char *fullpath, const char *prefix,
                                     size_t prefix_len,
                                     char **rpath, char **dest, const char *rpath_limit)
{
    int nret = 0;
    int num_links = 0;
    const char *start = NULL;
    const char *end = NULL;
    char *extra_buf = NULL;
    errno_t rc = EOK;

    start = fullpath + prefix_len;
    for (end = start; *start; start = end) {
        do_eval_symlinks_skip_slash(&start, &end);
        if (end - start == 0) {
            break;
        } else if (is_specify_current(end, start)) {
            continue;
        } else if (is_specify_parent(end, start)) {
            skip_dest_trailing_slash(dest, rpath, prefix_len);
        } else {
            if (!ISSLASH((*dest)[-1])) {
                *(*dest)++ = '/';
            }

            nret = do_path_realloc(start, end, rpath, dest, &rpath_limit);
            if (nret != 0) {
                nret = -1;
                goto out;
            }

            rc = memcpy_s(*dest, (size_t)(end - start), start, (size_t)(end - start));
            if (rc != EOK) {
                ERROR("Out of memory");
                nret = -1;
                goto out;
            }
            *dest += end - start;
            **dest = '\0';

            if (do_eval_symlinks_in_scope_is_symlink(*rpath)) {
                continue;
            }

            nret = do_get_symlinks(&fullpath, prefix, prefix_len, rpath, dest, &end, &num_links, &extra_buf);
            if (nret != 0) {
                nret = -1;
                goto out;
            }
        }
    }
out:
    free(extra_buf);
    return nret;
}

static char *eval_symlinks_in_scope(const char *fullpath, const char *rootpath)
{
    char *root = NULL;
    char *rpath = NULL;
    char *dest = NULL;
    char *prefix = NULL;
    const char *rpath_limit = NULL;
    size_t prefix_len;
    errno_t rc = EOK;
    char resroot[PATH_MAX] = { 0 };

    if (fullpath == NULL || rootpath == NULL) {
        return NULL;
    }

    root = cleanpath(rootpath, resroot, sizeof(resroot));
    if (root == NULL) {
        ERROR("Failed to get cleaned path");
        return NULL;
    }

    if (strcmp(fullpath, root) == 0) {
        return util_strdup_s(fullpath);
    }

    if (strstr(fullpath, root) == NULL) {
        ERROR("Path '%s' is not in '%s'", fullpath, root);
        return NULL;
    }

    rpath = util_common_calloc_s(PATH_MAX);
    if (rpath == NULL) {
        ERROR("Out of memory");
        goto out;
    }
    rpath_limit = rpath + PATH_MAX;

    prefix = root;
    prefix_len = (size_t)strlen(prefix);
    if (strcmp(prefix, "/") == 0) {
        prefix_len = 0;
    }

    dest = rpath;
    if (prefix_len) {
        rc = memcpy_s(rpath, PATH_MAX, prefix, prefix_len);
        if (rc != EOK) {
            ERROR("Out of memory");
            goto out;
        }
        dest += prefix_len;
    }
    *dest++ = '/';

    if (do_eval_symlinks_in_scope(fullpath, prefix, prefix_len, &rpath, &dest,
                                  rpath_limit)) {
        goto out;
    }

    if (dest > rpath + prefix_len + 1 && ISSLASH(dest[-1])) {
        --dest;
    }
    *dest = '\0';
    return rpath;

out:
    free(rpath);
    return NULL;
}

char *follow_symlink_in_scope(const char *fullpath, const char *rootpath)
{
    char resfull[PATH_MAX] = { 0 };
    char *full = NULL;
    char resroot[PATH_MAX] = { 0 };
    char *root = NULL;

    full = cleanpath(fullpath, resfull, sizeof(resfull));
    if (full == NULL) {
        ERROR("Failed to get cleaned path");
        return NULL;
    }

    root = cleanpath(rootpath, resroot, sizeof(resroot));
    if (root == NULL) {
        ERROR("Failed to get cleaned path");
        return NULL;
    }

    return eval_symlinks_in_scope(full, root);
}

bool specify_current_dir(const char *path)
{
    char *dup = NULL;
    char *base = NULL;
    bool res = false;

    if (path == NULL) {
        return false;
    }

    if (path[0] == '\0') {
        return true;
    }

    dup = util_strdup_s(path);
    base = basename(dup);
    res = (base != NULL) && (strcmp(base, ".") == 0);
    free(dup);
    return res;
}

bool has_trailing_path_separator(const char *path)
{
    return path != NULL && strlen(path) > 0 && (path[strlen(path) - 1] == '/');
}

static void set_char_to_separator(char *p)
{
    *p = '/';
}

char *preserve_trailing_dot_or_separator(const char *cleanedpath, const char *originalpath)
{
    int nret;
    char respath[PATH_MAX + 3] = { 0 };

    if (cleanedpath == NULL || originalpath == NULL) {
        return NULL;
    }

    if (cleanedpath[0] == '\0' || originalpath[0] == '\0') {
        return NULL;
    }

    nret = sprintf_s(respath, PATH_MAX, "%s", cleanedpath);
    if (nret < 0) {
        ERROR("Failed to print string");
        return NULL;
    }

    if (!specify_current_dir(cleanedpath) && specify_current_dir(originalpath)) {
        if (!has_trailing_path_separator(respath)) {
            set_char_to_separator(&respath[strlen(respath)]);
        }
        respath[strlen(respath)] = '.';
    }

    if (!has_trailing_path_separator(respath) && has_trailing_path_separator(originalpath)) {
        set_char_to_separator(&respath[strlen(respath)]);
    }

    return util_strdup_s(respath);
}

int filepath_split(const char *path, char **dir, char **base)
{
    ssize_t i;
    char *dup = NULL;

    if (path == NULL) {
        return -1;
    }

    dup = util_strdup_s(path);
    i = (ssize_t)strlen(dup) - 1;
    while (i >= 0 && dup[i] != '/') {
        i--;
    }

    if (i != -1) {
        char cache = dup[i + 1];
        dup[i + 1] = '\0';
        if (dir != NULL) {
            *dir = util_strdup_s(dup);
        }
        dup[i + 1] = cache;
    } else {
        if (dir != NULL) {
            *dir = util_strdup_s(".");
        }
    }

    if (base != NULL) {
        *base = util_strdup_s(dup + i + 1);
    }
    free(dup);
    return 0;
}

int split_dir_and_base_name(const char *path, char **dir, char **base)
{
    char *dupdir = NULL;
    char *dupbase = NULL;

    if (path == NULL) {
        return -1;
    }

    dupdir = util_strdup_s(path);
    dupbase = util_strdup_s(path);
    if (dir != NULL) {
        *dir = util_strdup_s(dirname(dupdir));
    }
    if (base != NULL) {
        *base = util_strdup_s(basename(dupbase));
    }
    free(dupdir);
    free(dupbase);
    return 0;
}

char *get_resource_path(const char *rootpath, const char *path)
{
    int nret;
    char tmppath[PATH_MAX] = { 0 };
    char fullpath[PATH_MAX] = { 0 };

    nret = sprintf_s(tmppath, sizeof(tmppath), "/%s/%s", rootpath, path);
    if (nret < 0) {
        return NULL;
    }

    if (cleanpath(tmppath, fullpath, sizeof(fullpath)) == NULL) {
        return NULL;
    }

    return follow_symlink_in_scope(fullpath, rootpath);
}

int resolve_path(const char *rootpath, const char *path, char **resolvedpath, char **abspath)
{
    int ret = -1;
    int nret;
    size_t len;
    char *dirpath = NULL;
    char *basepath = NULL;
    char *resolved_dir_path = NULL;
    char tmppath[PATH_MAX] = { 0 };
    char cleanedpath[PATH_MAX] = { 0 };

    *resolvedpath = NULL;
    *abspath = NULL;

    nret = sprintf_s(tmppath, sizeof(tmppath), "/%s", path);
    if (nret < 0) {
        ERROR("Failed to print string");
        return -1;
    }

    if (cleanpath(tmppath, cleanedpath, sizeof(cleanedpath)) == NULL) {
        ERROR("Failed to get cleaned path: %s", tmppath);
        return -1;
    }

    *abspath = preserve_trailing_dot_or_separator(cleanedpath, tmppath);
    if (*abspath == NULL) {
        ERROR("Failed to preserve path");
        goto cleanup;
    }

    nret = filepath_split(*abspath, &dirpath, &basepath);
    if (nret < 0) {
        ERROR("Failed to split path");
        goto cleanup;
    }

    resolved_dir_path = get_resource_path(rootpath, dirpath);
    if (resolved_dir_path == NULL) {
        ERROR("Failed to get resource path");
        goto cleanup;
    }
    len = strlen(resolved_dir_path) + strlen("/") + strlen(basepath) + (size_t)1;
    *resolvedpath = util_common_calloc_s(len);
    if (*resolvedpath == NULL) {
        ERROR("Out of memory");
        goto cleanup;
    }
    nret = sprintf_s(*resolvedpath, len, "%s/%s", resolved_dir_path, basepath);
    if (nret < 0) {
        ERROR("Failed to print string");
        goto cleanup;
    }

    ret = 0;
cleanup:
    free(dirpath);
    free(basepath);
    free(resolved_dir_path);

    if (ret != 0) {
        free(*abspath);
        *abspath = NULL;
        free(*resolvedpath);
        *resolvedpath = NULL;
    }
    return ret;
}

