/*
 * cagi-internals.c
 *
 * This file must be included in any C program that wishes to make use of the Asterisk AGI.
 * This source file contains internal functions which are used by the cagi.c functions to
 * implement the Asterisk AGI API.
 *
 * author:	Randall Degges
 * email:	rdegges@gmail.com
 * date:	6-21-09
 * license:	GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>	// va_ macros
#include <stdlib.h>
#include "wpbx-cagi.h"
#include "wpbx-cagi-internals.h"

/*
 * readvars
 *	Read in all asterisk pre-defined variables for the AGI script to use
 *	and store them in a asterisk_vars struct.
 * params
 *	none
 * returns
 *	Success: Populated asterisk_vars structure (which needs to be free()'d
 *		by the user.
 *	Failure: Quits the program with exit status 1.
 *
 */
asterisk_vars * readvars(void) {

	int i, j;
	char *value, *ptr;
	char buff[_BUFF_SIZE];
	asterisk_vars *session_vars;

	/*
	 * The session_vars is a asterisk_vars struct which holds all of the
	 * variables and their values that asterisk passes to our AGI script
	 * during initial execution. IT MUST BE FREE()'d BY THE USER AT PROGRAM
	 * TERMINATION!
	 */
	session_vars = safe_malloc(sizeof(struct asterisk_vars));

	fgets(buff, _BUFF_SIZE, stdin);

	/*
	 * Keep reading variables from stdin that asterisk is passing us until
	 * we have completely retrieved all values. Each variable is of the
	 * form:
	 *      variablename: value\n
	 *
	 * Asterisk signals that it is finished sending variables by sending a
	 * trailing \n character.
	 */
	for (i = 0, j = 0; buff[0] != '\n' && buff != NULL; i++) {

		/*
		 * All variable values follow a semicolon character, so remove
		 * the stuff before it and the space after it. Some values may
		 * be empty (a \n character), so check for that special case as
		 * well.
		 */
		value = strchr(buff, ':');
		if (value == NULL) {
			print_debug("ERROR! Problem reading variables.");
			exit(1);
		}
		value+=2;       // skip space chars, go directly to value

		/*
		 * Special case where the variable has no assigned value. In
		 * this case we assign a single space character ' ' to the
		 * variable as its value. We also remove the trailing \n from
		 * all variables.
		 */
		if (value[0] == '\n')
			value = " ";
		else {
			ptr = strchr(value, '\n');
			if (value == NULL) {
				print_debug("ERROR! Variable has incorrect "
						"formatting. Exiting.");
				exit(1);
			}
			*ptr = '\0';
		}

		/*
		 * Assign the incoming values to their correct variables and
		 * popular the variable structure. Our strncpy() uses the len
		 * argument as _BUFF_SIZE-1 to guarantee null-termination. =)
		 */
		switch (i) {
			case 0:
				strncpy(session_vars->agi_request, value,
								_BUFF_SIZE-1);
				break;
			case 1:
				strncpy(session_vars->agi_channel, value,
								_BUFF_SIZE-1);
				break;
			case 2:
				strncpy(session_vars->agi_language, value,
								_BUFF_SIZE-1);
				break;
			case 3:
				strncpy(session_vars->agi_type, value,
								_BUFF_SIZE-1);
				break;
			case 4:
				strncpy(session_vars->agi_uniqueid, value,
								_BUFF_SIZE-1);
				break;
			case 5:
				strncpy(session_vars->agi_version, value,
								_BUFF_SIZE-1);
				break;
			case 6:
				strncpy(session_vars->agi_callerid, value,
								_BUFF_SIZE-1);
				break;
			case 7:
				strncpy(session_vars->agi_calleridname, value,
								_BUFF_SIZE-1);
				break;
			case 8:
				strncpy(session_vars->agi_callingpres, value,
								_BUFF_SIZE-1);
				break;
			case 9:
				strncpy(session_vars->agi_callingani2, value,
								_BUFF_SIZE-1);
				break;
			case 10:
				strncpy(session_vars->agi_callington, value,
								_BUFF_SIZE-1);
				break;
			case 11:
				strncpy(session_vars->agi_callingtns, value,
								_BUFF_SIZE-1);
				break;
			case 12:
				strncpy(session_vars->agi_dnid, value,
								_BUFF_SIZE-1);
				break;
			case 13:
				strncpy(session_vars->agi_rdnis, value,
								_BUFF_SIZE-1);
				break;
			case 14:
				strncpy(session_vars->agi_context, value,
								_BUFF_SIZE-1);
				break;
			case 15:
				strncpy(session_vars->agi_extension, value,
								_BUFF_SIZE-1);
				break;
			case 16:
				strncpy(session_vars->agi_priority, value,
								_BUFF_SIZE-1);
				break;
			case 17:
				strncpy(session_vars->agi_enhanced, value,
								_BUFF_SIZE-1);
				break;
			case 18:
				strncpy(session_vars->agi_accountcode, value,
								_BUFF_SIZE-1);
				break;
			case 19:
				strncpy(session_vars->agi_threadid, value,
								_BUFF_SIZE-1);
				break;
			/*
			 * If we got to the default option, it means these are
			 * user-passed AGI variables. There can be at most
			 * _MAX_ARGS of these. And they are of the form:
			 * agi_arg_1 -> agi_arg_127. Since C standards dictate
			 * NOT to start arrays with 1, we will rename these
			 * args agi_arg_0 -> agi_arg_126.
			 */
			default:
				strncpy(session_vars->agi_args[j++], value,
								_BUFF_SIZE);
		}

		/*
		 * Continue reading from stdin until we have exhausted the list
		 * of variables that asterisk is passing to us.
		 */
		fgets(buff, _BUFF_SIZE, stdin);
	}

	return session_vars;

}

/*
 * print_debug
 *	Print debug messages to STDERR and flush the output. Auto adds a
 *	newline character (\n) to the end of the message. This will show up in
 *	the asterisk root console. (Useful debug tool).
 * params (required)
 *	<debugmsg>
 * returns
 *	Success: void.
 *	Failure: void.
 */
void print_debug(const char *debugmsg) {

	fprintf(stderr, "%s\n", debugmsg);
	fflush(stderr);

}

/*
 * safe_malloc
 *	Safely allocate memory. Will check for failure of allocation and quit
 *	the program if memory can't be allocated. (Typically means the server
 *	is under -extremely- high stress if this happens).
 * params (required)
 *	<size>
 *returns
 *	Success: An uninitialized void * object for storage.
 *	Failure: Quits the program with exit status 1.
 */
void * safe_malloc(const int size) {

	void *mem;

	if ((mem = malloc(size)) == NULL) {
		print_debug("ERROR! Cannot allocate memory. Exiting.");
		exit(1);
	}

	return mem;

}

/*
 * evaluate
 *	Evaluates an AGI command and returns useful information.
 * params (required)
 *	<command>
 * returns:
 *	Success: Returns a two-dimensional array of values. The values are:
 *		char *data[0] = <code>
 *		char *data[1] = <result>
 *		char *data[2] = <string with data OR empty string "">
 *	Failure: Quits the program with exit status 1.
 */
char ** evaluate(const char *command) {

	int i;
	char buff[_BUFF_SIZE];
	char *begin, *end, **data;

	/*
	 * The data array is an array of strings which will be returned to the
	 * calling function for processing. IT MUST BE FREE'd BY THE USER!
	 */
	data = safe_malloc(_RETURN_ELEMENTS * sizeof(char *));
	for (i = 0; i < _RETURN_ELEMENTS; i++)
		data[i] = safe_malloc(_BUFF_SIZE);

	/*
	 * Start by sending the command to asterisk to evaluate. Commands are
	 * sent raw, however they are passed to this function. Make sure that
	 * all commands are terminated with \n so that asterisk reads the
	 * command alone. We flush stdout and stdin to ensure that the command
	 * was received by asterisk, and also that asterisk's return data is
	 * put into stdin for our retrieval.
	 */
	fprintf(stdout, "%s", command);
	fflush(stdout);
	fflush(stdin);

	/*
	 * Read the returned data from asterisk. This is what we will parse to
	 * get the required information.
	 */
	fgets(buff, _BUFF_SIZE, stdin);

	/*
	 * The first thing we parse for is the code element. It is the first
	 * block of text before the space character ' '.
	 */
	begin = buff;
	end = strchr(buff, ' ');
	if (end == NULL) {
		print_debug("ERROR! Problem parsing input.");
		exit(1);
	}
	*end = '\0';
	end++;

	strncpy(data[0], begin, _BUFF_SIZE-1);

	/*
	 * Now we parse the result. It follows the code element by a space
	 * character. The actual result, however, directly follows the '='
	 * character.
	 */
	begin = strchr(end, '=');
	if (begin == NULL) {
		print_debug("ERROR! Problem parsing input.");
		exit(1);
	}
	begin++;

	/*
	 * Not all commands return the third (last) field. The last field is
	 * optionally returned by asterisk. If there is no space character,
	 * then we look for the trailing \n character which signals the end of
	 * data given to us by asterisk.
	 */
	end = strchr(begin, ' ');
	if (end == NULL) {

		/*
		 * If we reach this point in the code, this means that asterisk
		 * only passed us the code and result, and that the third
		 * option, data, was not specified. This means that we should
		 * end early since there is nothing else to read.
		 */
		end = strrchr(begin, '\n');
		if (end == NULL) {
			print_debug("ERROR! Problem parsing input.");
			exit(1);
		}
		*end = '\0';

		strncpy(data[1], begin, _BUFF_SIZE-1);
		strncpy(data[2], "", _BUFF_SIZE-1);

		return data;
	}
	*end = '\0';
	end++;

	strncpy(data[1], begin, _BUFF_SIZE-1);

	/*
	 * Now we parse the last values passed to us--the data. We'll pass all
	 * data as a single string. Don't forget, we don't return ANYTHING with
	 * a trailing \n character. We only return printable, null-terminated
	 * single line strings for clarity.
	 */
	begin = end;
	end = strrchr(begin, '\n');
	if (end == NULL) {
		print_debug("ERROR! Problem parsing input.");
		exit(1);
	}
	*end = '\0';

	strncpy(data[2], begin, _BUFF_SIZE-1);
	return data;

}

/*
 * free_2d_array
 *	Free's a two-dimensional array that has been malloc'ed. For internal
 *	use only.
 * returns
 *	Success: void
 *	Failure: void (will cause errors)
 */
void free_2d_array(char **data) {

	int i;

	/*
	 * Here we free each string which is stored as part of the string
	 * array.
	 */
	for (i = 0; i < _RETURN_ELEMENTS; i++)
		free(data[i]);

	free(data);

}

/*
 * format_str
 *	Takes in string paramaters and combines them into one large string.
 * params (required, required, optional)
 *	<number of arguments> <arg1> [<arg2>] [<argn...>]
 * returns
 *	Success: A string object which has been malloc'ed. IT MUST BE FREE'd by
 *		the user!
 *	Failure: Quits the program with exit status 1.
 */
char * format_str(const int count, const char *str1, ...) {

	int i, len;
	va_list args;
	char *str, *ptr, *tmp;

	/*
	 * Here we count the total amount of bytes (characters) that we need to
	 * allocate to hold all of the strings that the user passed to us. The
	 * string lengths are counted here, and we add 1 to the number to
	 * account for the trailing '\0' character which we use to null-
	 * terminate the string so that it will be printable.
	 */
	va_start(args, str1);
	for (i = 0, len = strlen(str1); i < count-1; i++)
		len += strlen(va_arg(args, char *));

	len++;
	va_end(args);

	str = safe_malloc(len);

	/*
	 * Now we loop through the arguments again, this time concatenating
	 * them into the larger string, which will then be returned to the
	 * user. If any strings are empty, skip them to avoid issues.
	 */
	va_start(args, str1);

	if (strcmp(str1, "") == 0)
		ptr = str;
	else {
		sprintf(str, "%s", str1);
		ptr = str + strlen(str1);
	}

	for (i = 0; i < count-1; i++) {
		tmp = va_arg(args, char *);
		if (strcmp(tmp, "") != 0) {
			sprintf(ptr, "%s", tmp);
			ptr += strlen(tmp);
		}
	}
	va_end(args);

	return str;

}

/*
 * create_dummy
 *	Create a two-dimensional array and return it to the user.
 * params (optional)
 *	<code> <result> <data>
 * returns
 *	Success: Returns a two-dimensional array of values. The values are:
 *		char *dummy[0] = <code>
 *		char *dummy[1] = <result>
 *		char *dummy[2] = <data>
 *	Failure: Quits the program with exit status 1.
 */
char ** create_dummy(const char *code, const char *result, const char *data) {

	int i;
	char **dummy;

	dummy = safe_malloc(_RETURN_ELEMENTS * sizeof(char *));
	for (i = 0; i < _RETURN_ELEMENTS; i++)
		dummy[i] = safe_malloc(_BUFF_SIZE);

	strcpy(dummy[0], code);
	strcpy(dummy[1], result);
	strcpy(dummy[2], data);

	return dummy;

}
