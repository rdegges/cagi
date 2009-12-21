/*
 * cagi-internals.h
 *
 * This file must be included in any C program that wishes to make use of the Asterisk AGI.
 * This header file contains all of the internal function declarations for use by users and
 * the cagi.c functions.
 *
 * author:	Randall Degges
 * email:	rdegges@gmail.com
 * date:	6-21-09
 * license:	GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <stdio.h>

asterisk_vars * readvars(void);
void print_debug(const char *debugmsg);
void * safe_malloc(const int size);
char ** evaluate(const char *command);
void free_2d_array(char **data);
char * format_str(const int count, const char *str1, ...);
char ** create_dummy(const char *code, const char *result, const char *data);
