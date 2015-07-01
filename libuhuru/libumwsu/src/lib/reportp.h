#ifndef _LIBUMWSU_REPORTP_H_
#define _LIBUMWSU_REPORTP_H_

#include <libumwsu/scan.h>

void umwsu_report_init(struct umwsu_report *report, const char *path);

void umwsu_report_destroy(struct umwsu_report *report);

void umwsu_report_change(struct umwsu_report *report, enum umwsu_file_status status, const char *mod_name, const char *mod_report);

#endif
