/*
    generic_stacktrace.h

    Copyright (C) 2013  Red Hat, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stacktrace.h"
#include "thread.h"

typedef struct sr_stacktrace* (*parse_fn_t)(const char **, char **);
typedef struct sr_stacktrace* (*parse_location_fn_t)(const char **, struct sr_location *);
typedef char* (*to_short_text_fn_t)(struct sr_stacktrace*, int);
typedef char* (*to_json_fn_t)(struct sr_stacktrace *);
typedef char* (*get_reason_fn_t)(struct sr_stacktrace *);
typedef struct sr_thread* (*find_crash_thread_fn_t)(struct sr_stacktrace *);
typedef void (*free_fn_t)(struct sr_stacktrace *);

struct stacktrace_methods
{
    parse_fn_t parse;
    parse_location_fn_t parse_location;
    to_short_text_fn_t to_short_text;
    to_json_fn_t to_json;
    get_reason_fn_t get_reason;
    find_crash_thread_fn_t find_crash_thread;
    free_fn_t free;
};

extern struct stacktrace_methods core_stacktrace_methods, python_stacktrace_methods,
       koops_stacktrace_methods, gdb_stacktrace_methods, java_stacktrace_methods;

/* XXX generic functions */
struct sr_stacktrace *
stacktrace_parse_wrapper(enum sr_report_type type, const char **input, char **error_message);

char *
stacktrace_to_short_text(struct sr_stacktrace *stacktrace, int max_frames);

struct sr_thread *
stacktrace_one_thread_only(struct sr_stacktrace *stacktrace);

struct sr_stacktrace *
stacktrace_parse_wrapper(enum sr_report_type type, const char **input, char **error_message);
