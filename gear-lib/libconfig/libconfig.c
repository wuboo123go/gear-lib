/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#include "libconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#if defined (__linux__) || defined (__CYGWIN__)
#include <unistd.h>
#elif defined (__WIN32__) || defined (WIN32) || defined (_MSC_VER)
#include "libposix4win.h"
#endif
#include <libmacro.h>
#include <errno.h>


extern struct config_ops ini_ops;
extern struct config_ops json_ops;
#ifdef ENABLE_LUA
extern struct config_ops lua_ops;
#endif

struct config *g_config = NULL;

struct config_ops_list {
    char suffix[32];
    struct config_ops *ops;
};

static struct config_ops_list conf_ops_list[] = {
#ifdef ENABLE_LUA
    {"lua", &lua_ops},
#endif
    {"json", &json_ops},
    {"ini", &ini_ops}
};

static char *get_file_suffix(const char *name)
{
    int point = '.';
    char *tmp = strrchr((char *)name, point);
    if (tmp) {
        return tmp+1;
    }
    return NULL;
}

static struct config_ops *find_backend(const char *name)
{
    int i = 0;
    int max_list;
    char *suffix;
    if (!name) {
        printf("config name can not be NULL\n");
        return NULL;
    }
    max_list = ARRAY_SIZE(conf_ops_list);
    suffix = get_file_suffix(name);
    if (!suffix) {
        printf("there is no suffix in config name\n");
        return NULL;
    }
    for (i = 0; i < max_list; i++) {
        if (!strcasecmp(conf_ops_list[i].suffix, suffix)) {
            break;
        }
    }
    if (i == max_list) {
        printf("the %s file is not supported\n", suffix);
        return NULL;
    }
    return conf_ops_list[i].ops;
}

struct config *conf_load(const char *name)
{
    struct config *c;
    struct config_ops *ops = find_backend(name);
    if (!ops) {
        printf("can not find valid config backend\n");
        return NULL;
    }
    c = CALLOC(1, struct config);
    if (!c) {
        printf("malloc failed!\n");
        return NULL;
    }
    c->ops = ops;
    if (c->ops->load) {
        if (-1 == c->ops->load(c, name)) {
            free(c);
            return NULL;
        }
    }
    g_config = c;
    return c;
}

int conf_set(struct config *c, const char *key, const char *val)
{
    if (!c || !c->ops->set_string)
        return -1;
    return c->ops->set_string(c, key, val, NULL);
}

void conf_del(struct config *c, const char *key)
{
    if (!c || !c->ops->del)
        return;
    c->ops->del(c, key);
}

void conf_dump(struct config *c)
{
    if (!c || !c->ops->dump)
        return;
    c->ops->dump(c, stderr);
}

int conf_save(struct config *c)
{
    if (!c || !c->ops->save)
        return -1;
    return c->ops->save(c);
}

void conf_dump_to_file(FILE *f, struct config *c)
{
    if (!c || !c->ops->dump)
        return;
    c->ops->dump(c, f);
}

void conf_unload(struct config *c)
{
    if (c && c->ops->unload) {
        c->ops->unload(c);
    }
    free(c);
}
