/*
    report.c

    Copyright (C) 2012  Red Hat, Inc.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA.
*/
#include "report.h"
#include "config.h"
#include "utils.h"
#include "koops_stacktrace.h"
#include "core_stacktrace.h"
#include "python_stacktrace.h"
#include "operating_system.h"
#include "rpm.h"
#include "strbuf.h"
#include <string.h>

struct btp_report *
btp_report_new()
{
    struct btp_report *report = btp_malloc(sizeof(struct btp_report));
    btp_report_init(report);
    return report;
}

void
btp_report_init(struct btp_report *report)
{
    report->report_version = 1;
    report->report_type = BTP_REPORT_INVALID;
    report->reporter_name = PACKAGE_NAME;
    report->reporter_version = PACKAGE_VERSION;
    report->user_root = false;
    report->user_local = true;
    report->operating_system = NULL;
    report->component_name = NULL;
    report->rpm_packages = NULL;
    report->python_stacktrace = NULL;
    report->koops_stacktrace = NULL;
    report->core_stacktrace = NULL;
}

void
btp_report_free(struct btp_report *report)
{
    free(report->component_name);
    btp_operating_system_free(report->operating_system);
    btp_rpm_package_free(report->rpm_packages, true);
    btp_python_stacktrace_free(report->python_stacktrace);
    btp_koops_stacktrace_free(report->koops_stacktrace);
    btp_core_stacktrace_free(report->core_stacktrace);
    free(report);
}

char *
btp_report_to_json(struct btp_report *report)
{
    struct btp_strbuf *strbuf = btp_strbuf_new();

    /* Report version. */
    btp_strbuf_append_strf(strbuf,
                           "{   \"report_version\": %"PRIu32"\n",
                           report->report_version);

    /* Report type. */
    const char *report_type;
    switch (report->report_type)
    {
    default:
    case BTP_REPORT_INVALID:
        report_type = "invalid";
        break;
    case BTP_REPORT_CORE:
        report_type = "core";
        break;
    case BTP_REPORT_PYTHON:
        report_type = "python";
        break;
    case BTP_REPORT_KERNELOOPS:
        report_type = "kerneloops";
        break;
    case BTP_REPORT_JAVA:
        report_type = "java";
        break;
    }

    btp_strbuf_append_strf(strbuf,
                           ",   \"report_type\": \"%s\"\n",
                           report_type);

    /* Reporter name. */
    if (report->reporter_name)
    {
        btp_strbuf_append_strf(strbuf,
                               ",   \"reporter_name\": \"%s\"\n",
                               report->reporter_name);
    }

    /* Reporter version. */
    if (report->reporter_version)
    {
        btp_strbuf_append_strf(strbuf,
                               ",   \"reporter_version\": \"%s\"\n",
                               report->reporter_version);
    }

    /* User type. */
    btp_strbuf_append_strf(strbuf,
                           ",   \"user_root\": %s\n",
                           report->user_root ? "true" : "false");

    btp_strbuf_append_strf(strbuf,
                           ",   \"user_local\": %s\n",
                           report->user_local ? "true" : "false");

    /* Operating system. */
    if (report->operating_system)
    {
        char *opsys_str = btp_operating_system_to_json(report->operating_system);
        char *opsys_str_indented = btp_indent_except_first_line(opsys_str, strlen(",   \"operating_system\": "));
        free(opsys_str);
        btp_strbuf_append_strf(strbuf,
                               ",   \"operating_system\": %s\n",
                               opsys_str_indented);

        free(opsys_str_indented);
    }

    /* Component name. */
    if (report->component_name)
    {
        btp_strbuf_append_strf(strbuf,
                               ",   \"component_name\": \"%s\"\n",
                               report->component_name);
    }

    /* RPM packages. */
    if (report->rpm_packages)
    {
        char *rpms_str = btp_rpm_package_to_json(report->rpm_packages, true);
        char *rpms_str_indented = btp_indent_except_first_line(rpms_str, strlen(",   \"rpm_packages\": "));
        free(rpms_str);
        btp_strbuf_append_strf(strbuf,
                               ",   \"rpm_packages\": %s\n",
                               rpms_str_indented);

        free(rpms_str_indented);
    }

    /* Core stacktrace. */
    if (report->core_stacktrace)
    {
        char *core_stacktrace_str = btp_core_stacktrace_to_json(report->core_stacktrace);
        char *core_stacktrace_str_indented = btp_indent_except_first_line(core_stacktrace_str, strlen(",   \"core_stacktrace\": "));
        free(core_stacktrace_str);
        btp_strbuf_append_strf(strbuf,
                               ",   \"core_stacktrace\": %s\n",
                               core_stacktrace_str_indented);

        free(core_stacktrace_str_indented);
    }

    btp_strbuf_append_str(strbuf, "}");
    return btp_strbuf_free_nobuf(strbuf);
}