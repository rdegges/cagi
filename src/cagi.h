/*
 * cagi.h
 *
 * This file may be included in any C program that wishes to make use of the Asterisk AGI.
 * This header file contains all asterisk AGI functionality for use by developers.
 *
 * MOTE: This has been written for Asterisk 1.6, and will not work with older versions of
 * Asterisk.
 *
 * author:	Randall Degges
 * email:	rdegges@gmail.com
 * date:	6-15-09
 * license:	GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <stdio.h>

/*
 * _BUFF_SIZE is the maximum amount of bytes that are allowed for variable 
 * values passed to AGI scripts. Ex: AGI(test,mylongvariablenameis500bytesmax)
 */
#ifndef _BUFF_SIZE
#define _BUFF_SIZE 500
#endif

/*
 * _MAX_ARGS is the maximum amount of arguments that an AGI script can have
 * passed to it via the AGI() dialplan application. Ex: AGI(test,1,2,3,...,127)
 * Any additional arguments will be concatenated together and thrown into the
 * last argument. (agi_arg_127)
 */
#ifndef _MAX_ARGS
#define _MAX_ARGS 127
#endif

/*
 * _RETURN_ELEMENTS is the amount of elements that asterisk returns after each
 * AGI command has been called. These elements are:
 *	<code> - An HTTP-like response code (200 for success, 5xx for error).
 *	<result> - Result of the command (common values are -1 for error, and 0
 *		for success)
 *	<data> - Some command return additional name=value pairs in data. See
 *		specific command documentation for more information.
 */
#ifndef _RETURN_ELEMENTS
#define _RETURN_ELEMENTS 3
#endif

/*
 * _DEFAULT_TIMEOUT is the amount of milli seconds that asterisk will default
 * to use for functions which require a timeout. (See get_data for an example).
 */
#ifndef _DEFAULT_TIMEOUT
#define _DEFAULT_TIMEOUT "2000"
#endif

/*
 * struct asterisk_vars
 *	A collection of pre-defined variables that asterisk sends to each AGI
 *	script.
 *
 * char agi_request[]:
 *	Name of the AGI script that is being called. Ex: myscript
 * char agi_channel[]:
 *	Channel that the call is coming from. Ex: Zap/1-1
 * char agi_language[]:
 *	Language that is configured on the server. Ex: en
 * char agi_type[]:
 *	Call type. Ex: SIP
 * char agi_uniqueid[]:
 *	A unique identifier for this session. Ex: 1245040107.63
 * char agi_version[]:
 *	Version of asterisk being ran. Ex: 1.6.0.9
 * char agi_callerid[]:
 *	Caller ID number. Ex: 101
 * char agi_calleridname[]:
 *	Caller ID name. Ex: Randall Degges
 * char agi_callingpres[]:
 *	PRI caller ID presentation variable. Ex: 0
 * char agi_callingani2[]:
 *	Caller ANI2 (PRI channels only). Ex: 0
 * char agi_callington[]:
 *	Caller type of number (PRI channels only). Ex: 0
 * char agi_callingtns[]:
 *	Ransit Network Selector (PRI channels only). Ex: 0
 * char agi_dnid[]:
 *	Dialed number identified. (Number dialed by agi_callerid). Ex: 102
 * char agi_rdnis[]:
 *	Redirected Dial Number ID Service. (The telephone number redirecting a
 *	call). Ex: unknown
 * char agi_context[]:
 *	Current context. Ex: default
 * char agi_extension[]:
 *	Extension that was called. Ex: 102
 * char agi_priority[]:
 *	Current priority in the dialplan. Ex: 1
 * char agi_enhanced[]:
 *	The flag value is 1.0 if started as an EAGI script. 0.0 otherwise.
 *	Ex: 0.0
 * char agi_accountcode[]:
 *	Account code. Ex:  <--- NOTE: This value may be a single space
 *	character. Decimal value 32, 0x20, ' '.
 * char agi_threadid[]:
 *	Thread ID of the AGI script (only in 1.6+). Ex: 139973785782592
 * char agi_args[][]:
 *	Array of arguments passed to the AGI script. There can be at most
 *	_MAX_ARGS arguments. Asterisk passes these arguments in the form
 *	agi_arg_1 -> agi_arg_127. These are renamed, however, to reflect C
 *	notation. They can be accessed via agi_args[0] -> agi_args[126]. Each
 *	argument is a null-terminated string that is guaranteed printable and
 *	has no newline (\n) characters.
 */
typedef struct asterisk_vars {
	char agi_request[_BUFF_SIZE];
	char agi_channel[_BUFF_SIZE];
	char agi_language[_BUFF_SIZE];
	char agi_type[_BUFF_SIZE];
	char agi_uniqueid[_BUFF_SIZE];
	char agi_version[_BUFF_SIZE];
	char agi_callerid[_BUFF_SIZE];
	char agi_calleridname[_BUFF_SIZE];
	char agi_callingpres[_BUFF_SIZE];
	char agi_callingani2[_BUFF_SIZE];
	char agi_callington[_BUFF_SIZE];
	char agi_callingtns[_BUFF_SIZE];
	char agi_dnid[_BUFF_SIZE];
	char agi_rdnis[_BUFF_SIZE];
	char agi_context[_BUFF_SIZE];
	char agi_extension[_BUFF_SIZE];
	char agi_priority[_BUFF_SIZE];
	char agi_enhanced[_BUFF_SIZE];
	char agi_accountcode[_BUFF_SIZE];
	char agi_threadid[_BUFF_SIZE];
	char agi_args[_MAX_ARGS][_BUFF_SIZE];
} asterisk_vars;

int answer(void);
int channel_status(const char *channel_name);
int database_del(const char *family, const char *key);
int database_deltree(const char *family, const char *keytree);
char * database_get(const char *family, const char *key);
int database_put(const char *family, const char *key, const char *value);
char ** exec(const char *application, const char *options);
char ** get_data(const char *file, const char *timeout, const char *maxdigits);
char * get_full_variable(const char *variablename, const char *channel);
char ** get_option(const char *file, const char *escapedigits, const char
								*timeout);
char * get_variable(const char *variablename);
int hangup(const char *channel_name);
int noop(const char *str);
char ** receive_char(const char *timeout);
char * receive_text(const char *timeout);
char **record_file(const char *file, const char *format, const char
	*escape_digits, const char *timeout, const char *offset_samples, const
					char *beep, const char *silence);
char * say_alpha(const char *letters, const char *escape_digits);
char * say_digits(const char *numbers, const char *escape_digits);
char * say_number(const char *number, const char *escape_digits, const char *
								gender);
char * say_phonetic(const char *string, const char *escape_digits);
char * say_date(const char *date, const char *escape_digits);
char * say_time(const char *time, const char *escape_digits);
char * say_datetime(const char *time, const char *escape_digits, const char
						*format, const char *timezone);
int send_image(const char *image);
int send_text(const char *text);
int set_autohangup(const char *time);
int set_callerid(const char *number);
int set_context(const char *context);
int set_extension(const char *extension);
int set_music(const char *onoff, const char *mclass);
int set_priority(const char *priority);
int set_variable(const char *variablename, const char *value);
char ** stream_file(const char *file, const char *escape_digits, const char
							*sample_offset);
char ** control_stream_file(const char *file, const char *escape_digits, const
	char *skipms, const char *ffchar, const char *rewchr, const char
								*pausechr);
int tdd_mode(const char *toggle);
int verbose(const char *message, const char *level);
int wait_for_digit(const char *timeout);
int speech_create(const char *engine);
int speech_set(const char *name, const char *value);
int speech_destroy(void);
int speech_load_grammar(const char *name, const char *path);
int speech_unload_grammar(const char *name);
int speech_activate_grammar(const char *name);
int speech_deactivate_grammar(const char *name);
char ** speech_recognize(const char *prompt, const char *timeout, const char
								*offset);
int gosub(const char *context, const char *extension, const char *priority,
							const char *arguments);
