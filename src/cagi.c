/*
 * cagi.c
 *
 * This file may be included in any C program that wishes to make use of the Asterisk AGI
 * library. This source file contains all Asterisk AGI functions which, paired with the
 * header file, give developers a complete AGI toolset to develop their own Asterisk AGI
 * programs.
 *
 * NOTE: This has been written for Asterisk 1.6, and will not work with older versions of
 * Asterisk.
 *
 * author:	Randall Degges
 * email:	rdegges@gmail.com
 * date:	6-15-2009
 * license:	GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wpbx-cagi.h"
#include "wpbx-cagi-internals.h"

/*
 * answer
 *	Answers the channel if it is not already in answered state.
 * link
 *	http://www.voip-info.org/wiki/view/answer
 * params
 *	none
 * returns
 *	Success: 0
 *	Failure: -1
 */
int answer(void) {

	int status;
	char **data;

	/*
	 * Asterisk will return:
	 * Success: 200 result=0
	 * Failure: 200 result=-1
	 */
	data = evaluate("ANSWER\n");
	if (strncmp(data[1], "-1", _BUFF_SIZE) == 0)
		status = -1;
	else
		status = 0;

	free_2d_array(data);
	return status;

}

/*
 * channel_status
 *	Returns the status of the specified channel.
 * link
 *	http://www.voip-info.org/wiki/view/channel+status
 * params (optional)
 *	[<channelname>]
 * returns
 *	Success: "200 result=<status>" Where status can be:
 *		0 - Channel is down and available.
 *		1 - Channel is down, but reserved.
 *		2 - Channel is off hook.
 *		3 - Digits (or equivalent) have been dialed.
 *		4 - Line is ringing.
 *		5 - Remote end is ringing.
 *		6 - Line is up.
 *		7 - Line is busy.
 *	Failure: -1
 */
int channel_status(const char *channel_name) {

	int status;
	char **data, *cmd;

	if (strcmp(channel_name, "") == 0)
		data = evaluate("CHANNEL STATUS\n");
	else {
		cmd = format_str(3, "CHANNEL STATUS ", channel_name, "\n");
		data = evaluate(cmd);
		free(cmd);
	}

	/*
	 * Set the correct return status of the command. If we get to the
	 * bottom, it means we failed.
	 */
	if (strcmp(data[1], "0") == 0)
		status = 0;
	else if (strcmp(data[1], "1") == 0)
		status = 1;
	else if (strcmp(data[1], "2") == 0)
		status = 2;
	else if (strcmp(data[1], "3") == 0)
		status = 3;
	else if (strcmp(data[1], "4") == 0)
		status = 4;
	else if (strcmp(data[1], "5") == 0)
		status = 5;
	else if (strcmp(data[1], "6") == 0)
		status = 6;
	else if (strcmp(data[1], "7") == 0)
		status = 7;
	else
		status = -1;

	free_2d_array(data);
	return status;

}

/*
 * database_del
 *	Deletes an entry in the Asterisk database for a given family and key.
 * link
 *	http://www.voip-info.org/wiki/view/database+del
 * params
 *	<family> <key>
 * returns
 *	Success: 1
 *	Failure: 0
 */
int database_del(const char *family, const char *key) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify a value for <family> or <key>, notify
	 * them and fail gracefully.
	 */
	if (strcmp(family, "") == 0) {
		print_debug("ERROR! <family> must not be empty.");
		return 0;
	} else if (strcmp(key, "") == 0) {
		print_debug("ERROR! <key> must not be empty.");
		return 0;
	}

	cmd = format_str(5, "DATABASE DEL ", family, " ", key, "\n");
	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result of the command is 1, it means that we successfully
	 * removed the key from the database. Otherwise, we failed.
	 */
	if (strcmp(data[1], "1") == 0)
		status = 1;
	else
		status = 0;

	free_2d_array(data);
	return status;

}

/*
 * database_deltree
 *	Deletes a family or specific keytree within a family in the Asterisk
 *	database.
 * link
 *	http://www.voip-info.org/wiki/view/database+deltree
 * params (required, optional)
 *	<family> [<keytree>]
 * returns
 *	Success: 1
 *	Failure: 0
 * notes
 *	Needs proper testing. If I delete a family ONLY, not specifying the
 *	keytree value, then it will always return true (like it doesn't care
 *	what I entered). I'm not sure if this is the desired behavior or a bug
 *	on my part, or a bug in asterisk 1.6.
 *
 *	Also--How do you make a keytree? What is a keytree? And once that is
 *	figured out, test the function to make sure it can delete keytree when
 *	it is specified.
 *
 *	-Randall (6/16/09)
 */
int database_deltree(const char *family, const char *keytree) {

	int status;
	char **data, *cmd;

	/*
	 * If <family> is not specified by the user, notify them and fail
	 * gracefully.
	 */
	if (strcmp(family, "") == 0) {
		print_debug("ERROR! <family> must not be empty.");
		return 0;
	}

	/*
	 * If the keytree value is not specified, then remove the entire
	 * family, otherwise, remove only the specific keytree.
	 */
	if (strcmp(keytree, "") == 0)
		cmd = format_str(3, "DATABASE DELTREE ", family, "\n");
	else
		cmd = format_str(5, "DATABASE DELTREE ", family, " ", keytree,
						"\n");

	data = evaluate(cmd);
	free(cmd);

	/*
	 * If asterisk deleted the family/keytree successfully, we return 1,
	 * otherwise we failed.
	 */
	if (strcmp(data[1], "1") == 0)
		status = 1;
	else
		status = 0;

	free_2d_array(data);
	return status;

}

/*
 * database_get
 *	Retrieves an entry in the Asterisk database for a given family and key.
 * link
 *	http://www.voip-info.org/wiki/view/database+get
 * params required
 *	<family> <key>
 * returns
 *	Success: Returns a string containing the value of the lookup.
 *	Failure: Returns an empty string.
 * NOTE: This array MUST BE FREED BY THE USER! Also--if this function fails
 *	because of user input, we don't even bother sending the command to
 *	asterisk, we just return the failed array. This will decrease execution
 *	time and increase stability :)
 */
char * database_get(const char *family, const char *key) {

	char **data, *cmd, *value;

	/*
	 * If either the <family> or <key> paramaters are empty, fail
	 * gracefully by returning a 'dumb' failed array. As mentioned above,
	 * the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(family, "") == 0) {
		print_debug("ERROR! <family> must not be empty.");
		value = safe_malloc(1);
		*value = '\0';
		return value;
	} else if (strcmp(key, "") == 0) {
		print_debug("ERROR! <key> must not be empty.");
		value = safe_malloc(1);
		*value = '\0';
		return value;
	}

	cmd = format_str(5, "DATABASE GET ", family, " ", key, "\n");
	data = evaluate(cmd);
	free(cmd);

	/*
	 * If we were able to get the value from the key, then return it as a
	 * string. Otherwise, we failed, so return an empty string.
	 */
	if (strcmp(data[1], "1") == 0) {
		value = safe_malloc(strlen(data[2]) + 1);
		strcpy(value, data[2]);
	} else {
		value = safe_malloc(1);
		*value = '\0';
	}

	free_2d_array(data);
	return value;

}

/*
 * database_put
 *	Adds or updates an entry in the Asterisk database for a given family,
 *	key, and value.
 * link
 *	http://www.voip-info.org/wiki/view/database+put
 * params required
 *	<family> <key> <value>
 * returns
 *	Success: 1
 *	Failure: 0
 */
int database_put(const char *family, const char *key, const char *value) {

	int status;
	char **data, *cmd;

	/*
	 * If either the <family> or <key> or <value> paramaters are empty,
	 * fail gracefully by returning a 'dumb' failed array. As mentioned
	 * above, the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(family, "") == 0) {
		print_debug("ERROR! <family> must not be empty.");
		return 0;
	} else if (strcmp(key, "") == 0) {
		print_debug("ERROR! <key> must not be empty.");
		return 0;
	} else if (strcmp(value, "") == 0) {
		print_debug("ERROR! <value> must not be empty.");
		return 0;
	}

	cmd = format_str(7, "DATABASE PUT ", family, " ", key, " ", value,
									"\n");
	data = evaluate(cmd);
	free(cmd);

	/*
	 * If asterisk put the new values into the astdb, then the result will
	 * be 1, so we can return it. Otherwise, we failed, so return 0.
	 */
	if (strcmp(data[1], "1") == 0)
		status = 1;
	else
		status = 0;

	free_2d_array(data);
	return status;

}

/*
 * exec
 *	Executes <application> with given <options>.
 * link
 *	http://www.voip-info.org/wiki/view/exec
 * params (required, optional)
 *	<application> <options>
 * returns
 *	Success: Returns a two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = <string with data OR empty string "">
 *		char *data[2] = ""
 *	Failure: Returns a two-dimensional array of values. The values are:
 *              char *data[0] = "200"
 *              char *data[1] = "-2"
 *              char *data[2] = ""
 *	NOTE: The array returned MUST BE FREED BY THE USER! Also--if this
 *		function fails because of user input, we don't even bother
 *		sending the command to asterisk, we just return the failed
 *		array. This will decrease execution time and increase stability
 *		:)
 */
char ** exec(const char *application, const char *options) {

	int escopt = 0, i, j;
	char **data, *cmd, *buff;

	/*
	 * If the <application> paramater is empty, fail gracefully by
	 * returning a 'dumb' failed array. Otherwise, execute the command. As
	 * mentioned above, the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(application, "") == 0) {
		print_debug("ERROR! <application> must not be empty.");
		data = create_dummy("200", "-2", "");
		return data;
	} else if (strcmp(options, "") == 0)
		cmd = format_str(3, "EXEC ", application, "\n");
	else {
		/*
		 * If the options string contains any spaces, we need to escape
		 * them since asterisk requires this.
		 */
		for (i = 0; i < strlen(options); i++)
			if (options[i] == ' ')
				escopt += 1;

		/*
		 * If the user string contains spaces, we now know how large
		 * the string needs to be in order to fix it. The extra byte is
		 * for null-termination.
		 */
		if (escopt) {
			FILE*f=fopen("/tmp/shit", "w");
			fprintf(f, "%d is the size of buff", 
				(int)strlen(options) + escopt + 1);
			fclose(f);

			buff = safe_malloc(strlen(options) + escopt + 1);

			/*
			 * i will keep track of the original options the user
			 * entered. j will keep track of our position in the
			 * buff (fixed copy).
			 */
			for (i = 0, j = 0; i < strlen(options); i++, j++) {
				if (options[i] == ' ') {
					buff[j++] = '\\';
					buff[j] = ' ';
				} else	
					/*
					 * If the current options letter had no
					 * spaces, just copy it over normally.
					 */
					buff[j] = options[i];
			}
			buff[j] = '\0';

			cmd = format_str(5, "EXEC ", application, " ", buff,
									"\n");
			free(buff);
		} else
			/*
			 * If we get here, it means that the option string had
			 * no spaces in it, so treat it normally.
			 */
			cmd = format_str(5, "EXEC ", application, " ", options,
									"\n");
	}

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * get_data
 *	Stream the given file, and recieve DTMF data. Returns the digits
 *	received from the channel at the other end.
 * link
 *	http://www.voip-info.org/wiki/view/get+data
 * params (required, optional, optional)
 *	<file to be streamed> [<timeout>] [<max digits>]
 * returns
 *	Success: Returns a two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = <string with digits entered by user OR empty
 *			string "">
 *		char *data[2] = ""
 *	Failure: Returns a two-dimensional array of values. These values are:
 *		If user reached <timeout> with pressed digits:
 *			char *data[0] = "200"
 *			char *data[1] = <string with digits entered by user OR
 *				empty string "">
 *			char *data[2] = <string with timeout OR empty string
 *				"">
 *		If user reached <timeout> without pressing any digits:
 *			char *data[0] = "200"
 *			char *data[1] = ""
 *			char *data[2] = <string with timeout OR empty string
 *				"">
 *	NOTE: The array returned MUST BE FREED BY THE USER! Also--if this
 *		function fails because of user input, we don't even bother
 *		sending the command to asterisk, we just return the failed
 *		array. This will decrease execution time and increase stability
 *		:)
 */
char ** get_data(const char *file, const char *timeout, const char *maxdigits){

	int given = 0;
	char **data, *cmd;

	/*
	 * If the <file> paramater is empty, fail gracefully by
	 * returning a 'dumb' failed array. Otherwise, execute the command. As
	 * mentioned above, the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(file, "") == 0) {
		print_debug("ERROR! <file> must not be empty.");
		data = create_dummy("200", "-1", "");
		return data;
	}

	if (strcmp(timeout, "") == 0)
		given = 1;

	/*
	 * If <timeout> or <maxdigits> is specified, then use them.
	 */
	if (strcmp(maxdigits, "") == 0) {
		cmd = format_str(5, "GET DATA ", file, " ",
			(given ? timeout : _DEFAULT_TIMEOUT), "\n");
	} else {
		cmd = format_str(7, "GET DATA ", file, " ",
			(given ? timeout : _DEFAULT_TIMEOUT), " ", maxdigits,
									"\n");
	}

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * get_full_variable
 *	Returns 0 if <variablename> is not set or channel does not exist.
 *	Returns 1 if <variablename> is set and returns the variable in
 *	parenthesis. Understands complex variable names and builtin variables,
 *	unlike GET VARIABLE.
 * link
 *	http://www.voip-info.org/wiki/view/get+full+variable
 * params (required, optional)
 *	<variablename> [<channel>]
 * returns
 *	Success: Returns the variable value as a string.
 *	Failure: Returns an empty string.
 *	NOTE: The array returned MUST BE FREED BY THE USER! Also--if this
 *		function fails because of user input, we don't even bother
 *		sending the command to asterisk, we just return the failed
 *		array. This will decrease execution time and increase stability
 *		:)
 *
 *		After much testing I discovered that this command will only
 *		fail if the channel specified does not exist. So be careful
 *		using this function, you must be certain that the variable
 *		exists before using it, otherwise you won't be able to tell
 *		whether or not you actually got its value reliably. -Randall
 */
char * get_full_variable(const char *variablename, const char *channel) {

	char **data, *cmd, *value;

	/*
	 * If the <variablename> paramater is empty, fail gracefully by
	 * returning a 'dumb' failed array. Otherwise, execute the command. As
	 * mentioned above, the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(variablename, "") == 0) {
		print_debug("ERROR! <variablename> cannot be empty.");
		value = safe_malloc(1);
		*value = '\0';
		return value;
	}
	
	/*
	 * If the channel is specified by the user, then use it. Otherwise,
	 * don't.
	 */
	if (strcmp(channel, "") == 0) {
		cmd = format_str(3, "GET FULL VARIABLE ", variablename, "\n");
	} else {
		cmd = format_str(5, "GET FULL VARIABLE ", variablename, " ",
								channel, "\n");
	}

	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result is 1, then we were able to get the variable, so return
	 * it. Otherwise, return an empty string.
	 */
	if (strcmp(data[1], "1") == 0) {
		value = safe_malloc(strlen(data[2]) + 1);
		strcpy(value, data[2]);
	} else {
		value = safe_malloc(1);
		*value = '\0';
	}

	free_2d_array(data);
	return value;

}

/*
 * get_option
 *	Behaves similar to STREAM FILE but used with a timeout option.
 * link
 *	http://www.voip-info.org/wiki/view/get+option
 * params (required, required, optional)
 *	<file> <escapedigits> [<timeout>]
 * returns
 *	Success: Returns a two-dimensional array of values. The values are:
 *		If no digits pressed:
 *			char *data[0] = "200"
 *			char *data[1] = "0"
 *			char *data[2] = <string with endpos=<offset> or empty
 *				string "">
 *		If digits pressed:
 *			char *data[0] = "200"
 *			char *data[1] = <string with digit OR empty string "">
 *			char *data[2] = <string with endpos=<offset> or emptry
 *				string "">
 *	Failure: Returns a two-dimensional array of values. The values are:
 *		Normal failure (various causes):
 *			char *data[0] = "200"
 *			char *data[1] = "-1"
 *			char *data[2] = <string with endpos=0 or empty string
 *				"">
 *		Failure on open:
 *			char *data[0] = "200"
 *			char *data[1] = "0"
 *			char *data[2] = <string with endpos=0 or empty string
 *				"">
 *	NOTE: The array returned MUST BE FREED BY THE USER! Also--if this
 *		function fails because of user input, we don't even bother
 *		sending the command to asterisk, we just return the failed
 *		array. This will decrease execution time and increase stability
 *		:)
 *	ALSO--it appears that when the user does hit one of the escape digits,
 *	when asterisk returns <digit> in the result field (*data[1]), it is in
 *	decimal ASCII value form. EG: If your escape digit is 1, and the user
 *	hits the 1 key while the <file> is being played, then (*data[1]) will
 *	have the value 49 (which is the decimal value of the ASCII digit '1').
 *	-Randall
 */
char ** get_option(const char *file, const char *escapedigits, const char
								*timeout) {

	char **data, *cmd;

	/*
	 * If <file> or <escapedigits> is empty, fail gracefully by returning a
	 * 'dumb' failed array. Otherwise, execute the command. As mentioned
	 * above, the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(file, "") == 0) {
		print_debug("ERROR! <file> must not be empty.");
		data = create_dummy("200", "-1", "endpos=0");
		return data;
	} else if (strcmp(escapedigits, "") == 0) {
		print_debug("ERROR! <escapedigits> must not be empty.");
		data = create_dummy("200", "-1", "endpos=0");
		return data;
	}

	if (strcmp(timeout, "") == 0)
		cmd = format_str(5, "GET OPTION ", file, " ", escapedigits,
									"\n");
	else
		cmd = format_str(7, "GET OPTION ", file, " ", escapedigits,
							" ", timeout, "\n");

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * get_variable
 *	Returns 0 if <variablename> is not set. Returns 1 if <variablename> is
 *	set and returns the variable in parenthesis.
 * link
 *	http://www.voip-info.org/wiki/view/get+variable
 * params (required)
 *	<variablename>
 * returns
 *	Success: Returns the value of the variable as a string.
 *	Failure: Returns an empty string.
 *	NOTE: The array returned MUST BE FREED BY THE USER! Also--if this
 *		function fails because of user input, we don't even bother
 *		sending the command to asterisk, we just return the failed
 *		array. This will decrease execution time and increase stability
 *		:)
 */
char * get_variable(const char *variablename) {

	char **data, *cmd, *value;

	/*
	 * If the <variablename> paramater is empty, fail gracefully by
	 * returning a 'dumb' failed array. Otherwise, execute the command. As
	 * mentioned above, the array MUST BE FREED BY THE USER!
	 */
	if (strcmp(variablename, "") == 0) {
		print_debug("ERROR! <variablename> cannot be empty.");
		value = safe_malloc(1);
		*value = '\0';
		return value;
	}

	cmd = format_str(3, "GET VARIABLE ", variablename, "\n");
        data = evaluate(cmd);
	free(cmd);

	/*
	 * If we were able to get the variable's value, then return it.
	 * Otherwise, we failed, so return the empty string.
	 */
	if (strcmp(data[1], "1") == 0) {
		value = safe_malloc(strlen(data[2]) + 1);
		strcpy(value, data[2]);
	} else {
		value = safe_malloc(1);
		*value = '\0';
	}

	free_2d_array(data);
	return value;

}

/*
 * hangup
 *	Hangs up the specified channel. If no channel name is given, hangs up
 *	the current channel
 * link
 *	http://www.voip-info.org/wiki/view/hangup
 * params (optional)
 *	[<channelname>]
 * returns
 *	Success: 1
 *	Failure: -1
 * NOTE: This command appears to have odd behavior occasionally. Sometimes,
 *	when I use it, it forces my script to return -1 instead of 0. It will
 *	also prevent any other commands from executing. Haven't found a
 *	workaround for this yet. Not sure what is causing this. -Randall
 */
int hangup(const char *channel_name) {

	int status;
	char **data, *cmd;

	if (strcmp(channel_name, "") == 0) {
		data = evaluate("HANGUP\n");
	} else {
		cmd = format_str(3, "HANGUP ", channel_name, "\n");
		data = evaluate(cmd);
		free(cmd);
	}

	/*
	 * Set the correct return status of the command. If the result is 1, it
	 * means we succeeded, otherwise, we failed.
	 */
	if (strcmp(data[1], "1") == 0)
		status = 1;
	else
		status = -1;

	free_2d_array(data);
	return status;

}

/*
 * noop
 *	Does nothing. Useful for debugging.
 * link
 *	http://www.voip-info.org/wiki/view/noop
 * params (optional)
 *	<str>
 * returns
 *	Success: 0
 *	Failure: never fails :)
 */
int noop(const char *str) {

	char **data, *cmd;

	if (strcmp(str, "") == 0) {
		data = evaluate("NOOP\n");
	} else {
		cmd = format_str(3, "NOOP ", str, "\n");
		data = evaluate(cmd);
		free(cmd);
	}

	free_2d_array(data);
	return 0;

}

/*
 * receive_char
 *	Receives a character of text on a channel. Specify timeout to be the
 *	maximum time to wait for input in milliseconds, or 0 for infinite. Most
 *	channels do not support the reception of text. Return the decimal value
 *	of the character if one is received, or 0 if the channel does not
 *	support text reception. Returns -1 only on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/receive+char
 * params (optional)
 *	[<timeout>]
 * returns
 *	Success: Returns a two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = <string with char received OR empty string "">
 *		char *data[2] = ""
 *	Failure: Returns a two-dimensional array of values. The values are:
 *		Failure or hangup:
 *			char *data[0] = "200"
 *			char *data[1] = "-1"
 *			char *data[2] = "(hangup)"
 *		Timeout:
 *			char *data[0] = "200"
 *			char *data[1] = <string with char received OR empty
 *				string "">
 *			char *data[2] = "(timeout)"
 *	NOTE: It appears that this command is BROKEN. Do NOT use it in your
 *		code as it will kill your channel after being called, and
 *		doesn't execute properly. I'm going to continue to investigate
 *		this, but I'm about 75% sure this is an asterisk bug. -Randall
 */
char ** receive_char(const char *timeout) {

	char **data, *cmd;
	FILE *f = fopen("/tmp/wtf", "w");

	/*
	 * If <timeout> isn't specified, use the default timeout (defined in
	 * the header file).
	 */
	if (strcmp(timeout, "") == 0)
		cmd = format_str(3, "RECEIVE CHAR ", _DEFAULT_TIMEOUT, "\n");
	else
		cmd = format_str(3, "RECEIVE CHAR ", timeout, "\n");

	data = evaluate(cmd);
	free(cmd);

	fprintf(f, "---\n");
	fprintf(f, "data[0]=%s\n", data[0]);
	fprintf(f, "data[1]=%s\n", data[1]);
	fprintf(f, "data[2]=%s\n", data[2]);
	fprintf(f, "---\n");
	//fgets(data[0], 500, stdin);
	//fprintf(f, "xxx=%s", data[0]);
	fclose(f);

	return data;

}

/*
 * receive_text
 *	Receives a string of text on a channel. Specify timeout to be the
 *	maximum time to wait for input in milliseconds, or 0 for infinite. Most
 *	channels do not support the reception of text. Returns -1 for failure
 *	or 1 for success, and the string in parentheses.
 * link
 *	http://www.voip-info.org/wiki/view/receive+text
 * params (optional)
 *	[<timeout>]
 * returns
 *	Success: Returns the text.
 *	Failure: Returns and empty string "".
 *	NOTE: If you specify 0 as the timeout, it will NOT listen for an
 *		infinite amount of time. It will instantly end. This is the new
 *		(undocumented) asterisk behavior. -Randall
 */
char * receive_text(const char *timeout) {

	char **data, *cmd, *value;

	/*
	 * If <timeout> isn't specified, use the default timeout (defined in
	 * the header file).
	 */
	if (strcmp(timeout, "") == 0)
		cmd = format_str(3, "RECEIVE TEXT ", _DEFAULT_TIMEOUT, "\n");
	else
		cmd = format_str(3, "RECEIVE TEXT ", timeout, "\n");

	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result field contains text, then return it. Otherwise, we
	 * failed, so return the empty string.
	 */
	if (strcmp(data[1], "-1") != 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(1);
		*value = '\0';
	}

	free_2d_array(data);
	return value;

}

/*
 * record_file
 *	Record to a file until a given DTMF digit in the sequence is received.
 *	Returns -1 on hangup or error. The format will specify what kind of
 *	file will be recorded. The timeout is the maximum record time in
 *	milliseconds, or -1 for no timeout. "Offset samples" is optional, and,
 *	if provided, will seek to the offset without exceeding the end of the
 *	file. "Silence" is the number of seconds of silence allowed before the
 *	function returns despite the lack of DTMF digits or reaching timeout.
 *	Silence value must be preceeded by "s=" and is also optional.
 * link
 *	http://www.voip-info.org/wiki/view/record+file
 * params (required, required, required, required, required, optional,
 *		optional, optional)
 *	<file> <format> <escape_digits> <timeout> [<offset_samples>]
 *		[BEEP] [s=<silence>]
 * returns
 *	Success: See Failure.
 *	Failure:
 *		Failure to write:
 *			char *data[0] = "200"
 *			char *data[1] = "-1"
 *			char *data[2] = "(writefile)"
 *		Failure on waitfor:
 *			char *data[0] = "200"
 *			char *data[1] = "-1"
 *			char *data[2] = "(waitfor) endpos=<offset>"
 *		Hangup:
 *			char *data[0] = "200"
 *			char *data[1] = "0"
 *			char *data[2] = "(hangup) endpos=<offset>"
 *		Interrupted:
 *			char *data[0] = "200"
 *			char *data[1] = "<digit>"
 *			char *data[2] = "(dtmf) endpos=<offset>"
 *		Timeout:
 *			char *data[0] = "200"
 *			char *data[1] = "0"
 *			char *data[2] = "(timeout) endpos=<offset>"
 *		Random error:
 *			char *data[0] = "200"
 *			char *data[1] = "<error>"
 *			char *data[2] = "(randomerror) endpos=<offset>"
 */
char ** record_file(const char *file, const char *format, const char
	*escape_digits, const char *timeout, const char *offset_samples, const
					char *beep, const char *silence) {

	char **data, *cmd;

	/*
	 * If any of the required params are not given, exit quickly by failing
	 * without passing to asterisk. This greatly reduces execution time.
	 */
	if (strcmp(file, "") == 0) {
		print_debug("ERROR! <file> must not be empty.");
		data = create_dummy("200", "-1", "(randomerror) endpos=0");
		return data;
	} else if (strcmp(format, "") == 0) {
		print_debug("ERROR! <format> must not be empty.");
		data = create_dummy("200", "-1", "(randomerror) endpos=0");
		return data;
	} else if (strcmp(escape_digits, "") == 0) {
		print_debug("ERROR! <escape_digits> must not be empty.");
		data = create_dummy("200", "-1", "(randomerror) endpos=0");
		return data;
	} else if (strcmp(timeout, "") == 0) {
		print_debug("ERROR! <timeout> must not be empty.");
		data = create_dummy("200", "-1", "(randomerror) endpos=0");
		return data;
	}

	if (strcmp(offset_samples, "") == 0) {
		cmd = format_str(9, "RECORD FILE ", file, " ", format, " ",
					escape_digits, " ", timeout, "\n");
	} else if (strcmp(beep, "") == 0) {
		cmd = format_str(11, "RECORD FILE ", file, " ", format, " ",
			escape_digits, " ", timeout, " ", offset_samples,
									"\n");
	} else if (strcmp(silence, "") == 0) {
		cmd = format_str(13, "RECORD FILE ", file, " ", format, " ",
			escape_digits, " ", timeout, " ", offset_samples,
							" ", beep, "\n");
	} else {
		cmd = format_str(15, "RECORD FILE ", file, " ", format, " ",
			escape_digits, " ", timeout, " ", offset_samples,
						" ", beep, " ", silence, "\n");
	}

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * say_alpha
 *	Say a given character string, returning early if any of the given DTMF
 *	digits are received on the channel. Returns 0 if playback completes
 *	without a digit being pressed, or the ASCII numerical value of the
 *	digit if one was perssed or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+alpha
 * params (required, required)
 *	<letters> <escape_digits>
 * returns
 *	Success: Returns a string with 0 in it. "0"
 *		Digit pressed: Returns a string with the digit in it.
 *	Failure: Returns a string with -1 in it. "-1"
 */
char * say_alpha(const char *letters, const char *escape_digits) {

	char **data, *cmd, *value;

	/*
	 * If the parameters aren't specified, quit quickly to save on
	 * execution time.
	 */
	if (strcmp(letters, "") == 0) {
		print_debug("ERROR! <letter> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	} else if (strcmp(escape_digits, "") == 0) {
		print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	}

	cmd = format_str(5, "SAY ALPHA ", letters, " ", escape_digits, "\n");
	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
	return value;

}

/*
 * say_digits
 *	Say a given digit string, returning early if any of the given DTMF
 *	digits are received on the channel. Returns 0 if playback completes
 *	without a digit being pressed, or the ASCII numerical value of the
 *	digit if one was pressed or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+digits
 * params (required, required)
 *	<numbers> <escape_digits>
 * returns
 *	Success: Returns a string with 0 in it. "0"
 *		Digit pressed: Returns a string with the digit in it.
 *	Failure: Returns a string with -1 in it. "-1"
 */
char * say_digits(const char *numbers, const char *escape_digits) {

	char **data, *cmd, *value;

	/*
	 * If any of the parameters weren't specified by the user, exit quickly
	 * for speed purposes.
	 */
	if (strcmp(numbers, "") == 0) {
		print_debug("ERROR! <numbers> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	} else if (strcmp(escape_digits, "") == 0) {
		print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	}

	cmd = format_str(5, "SAY DIGITS ", numbers, " ", escape_digits, "\n");
	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
	return value;

}

/*
 * say_number
 *	Say a given number, returning early if any of the given DTMF digits are
 *	received on the channel. Returns 0 if playback completes without a
 *	digit being pressed, or the ASCII numerical value of the digit if one
 *	was pressed or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+number
 * params (required, required, optional)
 *	<number> <escape_digits> [<gender>]
 * returns
 *	Success: Returns the string "0"
 *		Digit pressed: Returns a string with the digit in it.
 *	Failure: Returns a string with the digit in it.
 *	NOTE: I can't figure out what the optional gender argument takes in
 *		order to make it play a male voice. I tried M, m, MALE, male,
 *		boy, BOY, b, and B, none of which worked. If you figure it out,
 *		let me know. -Randall
 */
char * say_number(const char *number, const char *escape_digits, const char *
								gender) {

	char **data, *cmd, *value;

	/*
	 * If any of the parameters weren't specified by the user, exit quickly
	 * for speed purposes.
	 */
	if (strcmp(number, "") == 0) {
		print_debug("ERROR! <number> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	} else if (strcmp(escape_digits, "") == 0) {
		print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	}

	if (strcmp(gender, "") == 0) {
		cmd = format_str(5, "SAY NUMBER ", number, " ", escape_digits,
									"\n");
	} else {
		cmd = format_str(7, "SAY NUMBER ", number, " ", escape_digits,
							" ", gender, "\n");
	}

	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
	return value;

}

/*
 * say_phonetic
 *	Say a given character string with phonetics, returning early if any of
 *	the given DTMF digits are received on the channel. Returns 0 if
 *	playback completes without a digit pressed, the ASCII numerical value
 *	of the digit if one was pressed, or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+phonetic
 * params (required, required)
 *	<string> <escape_digits>
 * returns
 * Success: Returns the string "0"
 *	Digit pressed: Returns a string with the digit in it.
 * Failure: Returns the string "-1"
 */
char * say_phonetic(const char *string, const char *escape_digits) {

	char **data, *cmd, *value;

	/*
	 * If any of the parameters weren't specified by the user, exit quickly
	 * for speed purposes.
	 */
	if (strcmp(string, "") == 0) {
		print_debug("ERROR! <string> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	} else if (strcmp(escape_digits, "") == 0) {
		print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	}

	cmd = format_str(5, "SAY PHONETIC ", string, " ", escape_digits, "\n");
	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
	return value;

}

/*
 * say_date
 *	Say a given date, returning early if any of the given DTMF digits are
 *	received on the channel. Returns 0 if playback completes without a
 *	digit pressed, the ASCII numerical value of the digit if one was
 *	pressed, or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+date
 * params (required, required)
 *	<date> <escape_digits>
 * returns
 *	Success: Returns the string "0".
 *		Digit pressed: Returns a string with the digit in it.
 *	Failure: Returns the string "-1".
 */
char * say_date(const char *date, const char *escape_digits) {

        char **data, *cmd, *value;

        /*
         * If any of the parameters weren't specified by the user, exit quickly
         * for speed purposes.
         */
        if (strcmp(date, "") == 0) {
                print_debug("ERROR! <date> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
        } else if (strcmp(escape_digits, "") == 0) {
                print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
        }

        cmd = format_str(5, "SAY DATE ", date, " ", escape_digits, "\n");
        data = evaluate(cmd);
        free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
        return value;

}

/*
 * say_time
 *	Say a given time, returning early if any of the given DTMF digits are
 *	received on the channel. Returns 0 if playback completes without a
 *	digit pressed, the ASCII numerical value of the digit if one was
 *	pressed, or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+time
 * params (required, required)
 *      <date> <escape_digits>
 * returns
 * Success: Returns the string "0".
 *	Digit pressed: Returns a string with the digit in it.
 * Failure: Returns the string "-1".
 */
char * say_time(const char *time, const char *escape_digits) {

        char **data, *cmd, *value;

        /*
         * If any of the parameters weren't specified by the user, exit quickly
         * for speed purposes.
         */
        if (strcmp(time, "") == 0) {
                print_debug("ERROR! <time> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
        } else if (strcmp(escape_digits, "") == 0) {
                print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
        }

        cmd = format_str(5, "SAY TIME ", time, " ", escape_digits, "\n");
        data = evaluate(cmd);
        free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
        return value;

}

/*
 * say_datetime
 *      Say a given time, returning early if any of the given DTMF digits are
 *      received on the channel. <time> is number of seconds elapsed since
 *	00:00:00 on January 1, 1970, Coordinated Universal Time (UTC). [format]
 *	is the format the time should be said in. See voicemail.conf (defaults
 *	to "ABdY 'digits/at' IMp"). Acceptable values for [timezone] can be
 *	found in /usr/share/zoneinfo. Defaults to machine default. Returns 0
 *	if playback completes without a digit pressed, the ASCII numerical
 *	value of the digit if one was pressed, or -1 on error/hangup.
 * link
 *	http://www.voip-info.org/wiki/view/say+datetime
 * params (required, required, optional, optional)
 *	<time> <escape_digits> [<format>] [<timezone>]
 * returns
 * Success: Returns the string "0".
 *	Digit pressed: Returns a string with the digit in it.
 * Failure: Returns the string "-1".
 */
char * say_datetime(const char *time, const char *escape_digits, const char
					*format, const char *timezone) {

	char **data, *cmd, *value;

	/*
	 * If any of the parameters weren't specified by the user, exit quickly
	 * for speed purposes.
	 */
	if (strcmp(time, "") == 0) {
		print_debug("ERROR! <time> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	} else if (strcmp(escape_digits, "") == 0) {
		print_debug("ERROR! <escape_digits> must not be empty.");
		value = safe_malloc(strlen("-1") + 1);
		strcpy(value, "-1");
		return value;
	}

	if (strcmp(format, "") == 0) {
		cmd = format_str(5, "SAY DATETIME ", time, " ", escape_digits,
									"\n");
	} else if (strcmp(timezone, "") == 0) {
		cmd = format_str(7, "SAY DATETIME ", time, " ", escape_digits,
							" ", format, "\n");
	} else {
        	cmd = format_str(9, "SAY DATETIME ", time, " ", escape_digits,
					" ", format, " ", timezone, "\n");
	}

	data = evaluate(cmd);
	free(cmd);

	/*
	 * If the result is -1 we failed. If the result is 0, we succeeded, but
	 * the user didn't enter any digits. Otherwise, we got a digit from the
	 * user, so return it.
	 */
	if (strcmp(data[1], "-1") == 0) {
		value = safe_malloc(1);
		*value = '\0';
	} else if (strcmp(data[1], "0") == 0) {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	} else {
		value = safe_malloc(strlen(data[1]) + 1);
		strcpy(value, data[1]);
	}

	free_2d_array(data);
	return value;

}

/*
 * send_image
 *	Sends the given image on a channel. Most channels do not suppor the
 *	transmission of images. Returns 0 if image is sent, or if the channel
 *	does not support image transmission. Returns -1 only on error/hangup.
 *	Image names should not include extensions.
 * link
 *	http://www.voip-info.org/wiki/view/send+image
 * params (required)
 *	<image>
 * returns
 *	Success: 0
 *	Failure: -1
 * NOTE: When testing this, I found that the command always seems to return 0,
 *	even when you CANNOT send images over the channel... Needs to be
 *	further investigated. -Randall
 */
int send_image(const char *image) {

	int status;
	char **data, *cmd;

	/*
	 * If <image> isn't specified, quit early and save processing time.
	 */
	if (strcmp(image, "") == 0) {
		print_debug("ERROR! <image> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "SEND IMAGE ", image, "\n");
	data = evaluate(cmd);
	free(cmd);

	if (strcmp(data[1], "0") == 0)
		status = 0;
	else
		status = -1;

	free_2d_array(data);
	return status;

}

/*
 * send_text
 *	Sends the given text on a channel. Most channels do not support the
 *	transmission of text. Returns 0 if text is sent, or if the channel does
 *	not support text transmission. Returns -1 only on error/hangup. Text
 *	consisting of greater than one word should be placed in quotes since
 *	the command only accepts a single argument.
 * link
 *	http://www.voip-info.org/wiki/view/send+text
 * params (required)
 *	<text>
 * returns
 *	Success: 0
 *	Failure: -1
 */
int send_text(const char *text) {

	int status;
	char **data, *cmd;

	/*
	 * If <text> isn't specified, quit early and save processing time.
	 */
	if (strcmp(text, "") == 0) {
		print_debug("ERROR! <text> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "SEND TEXT \"", text, "\"\n");
	data = evaluate(cmd);
	free(cmd);

	if (strcmp(data[1], "0") == 0)
		status = 0;
	else
		status = -1;

	free_2d_array(data);
	return status;

}

/*
 * set_autohangup
 *	Cause the channel to automatically hangup at <time> seconds in the
 *	future. Of course, it can be hungup before then as well. Seting to 0
 *	will cause the autohangup feature to be disabled on this channel.
 * link
 *	http://www.voip-info.org/wiki/view/set+autohangup
 * params (required)
 *	<time>
 * returns
 *	Success: 0
 *	Failure: Never fails. :)
 */
int set_autohangup(const char *time) {

	char **data, *cmd;

	/*
	 * If <time> isn't specified, quit early and save processing time.
	 */
	if (strcmp(time, "") == 0) {
		print_debug("ERROR! <time> must not be empty.");
		return 0;
	}

	cmd = format_str(3, "SET AUTOHANGUP ", time, "\n");
	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 0;

}

/*
 * set_callerid
 *	Changes the CallerID of the current channel.
 * link
 *	http://www.voip-info.org/wiki/view/set+callerid
 * params (required)
 *	<number>
 * returns
 *      Success: 1
 *      Failure: Never fails. :)
 */
int set_callerid(const char *number) {

	char **data, *cmd;

	/*
	 * If <number> isn't specified, quit early and save processing time.
	 */
	if (strcmp(number, "") == 0) {
		print_debug("ERROR! <number> must not be empty.");
		return 1;
	}

	cmd = format_str(3, "SET CALLERID ", number, "\n");
	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 1;

}

/*
 * set_context
 *	Sets the context for continuation upon exiting the application.
 * link
 *	http://www.voip-info.org/wiki/view/set+context
 * params (required)
 *	<context>
 * returns
 *	Success: 0
 *	Failure: 0
 * NOTE: Don't specify an invalid context or the call will drop. I'm also not
 *	able to produce a working context. I always get an error. Need to do
 *	more testing to produce a case where it will work. -Randall
 */
int set_context(const char *context) {

	char **data, *cmd;

	/*
	 * If <context> isn't specified, quit early and save processing time.
	 */
	if (strcmp(context, "") == 0) {
		print_debug("ERROR! <context> must not be emnpty.");
		return 0;
	}

	cmd = format_str(3, "SET CONTEXT ", context, "\n");
	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 0;

}

/*
 * set_extension
 *	Changes the extension for continuation upon exiting the application.
 * link
 *	http://www.voip-info.org/wiki/view/set+extension
 * params (required)
 *	<extension>
 * returns
 *	Success: 0
 *	Failure: Never fails. :)
 */
int set_extension(const char *extension) {

	char **data, *cmd;

	/*
	 * If <extension> isn't specified, quit early and save processing time.
	 */
	if (strcmp(extension, "") == 0) {
		print_debug("ERROR! <extension> must not be empty.");
		return 0;
	}

	cmd = format_str(3, "SET EXTENSION ", extension, "\n");
	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 0;

}

/*
 * set_music
 *	Enables/disables the music on hold generator. If <class> is not
 *	specified, then the default music on hold class will be used.
 * link
 *	http://www.voip-info.org/wiki/view/set+music
 * params (required, optional)
 *	<ON|OFF> <class>
 * returns
 *	Success: 0
 *	Failure: 0
 */
int set_music(const char *onoff, const char *mclass) {

	char **data, *cmd;

	/*
	 * If <onoff> isn't specified, quit early and save processing time.
	 */
	if (strcmp(onoff, "") == 0) {
		print_debug("ERROR! <onoff> must not be empty.");
		return 0;
	}

	if (strcmp(mclass, "") == 0)
		cmd = format_str(3, "SET MUSIC ", onoff, "\n");
	else
		cmd = format_str(5, "SET MUSIC ", onoff, " ", mclass, "\n");

	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 0;

}

/*
 * set_priority
 *	Changes the priority for continuation upon exiting the application. The
 *	priority must be a valid priority or label.
 * link
 *	http://www.voip-info.org/wiki/view/set+priority
 * params (required)
 *	<priority>
 * returns
 *	Success: 0
 *	Failure: Never fails :)
 * NOTE: Must specify a valid priority, otherwise, program execution will stop.
 */
int set_priority(const char *priority) {

	char **data, *cmd;

	/*
	 * If <num> isn't specified, quit early and save processing time.
	 */
	if (strcmp(priority, "") == 0) {
		print_debug("ERROR! <priority> must not be empty.");
		return 0;
	}

	cmd = format_str(3, "SET PRIORITY ", priority, "\n");
	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 0;

}

/*
 * set_variable
 *	Sets a variable value.
 * link
 *	http://www.voip-info.org/wiki/view/set+variable
 * params (required)
 *	<variablename> <value>
 * returns
 *	Success: 1
 *	Failure: 1
 */
int set_variable(const char *variablename, const char *value) {

	char **data, *cmd;

	/*
	 * If <variablename> or <value> is not set, return early to save
	 * processing time.
	 */
	if (strcmp(variablename, "") == 0) {
		print_debug("ERROR! <variablename> must not be empty.");
		return 1;
	} else if (strcmp(value, "") == 0) {
		print_debug("ERROR! <value> must not be empty.");
		return 1;
	}

	cmd = format_str(5, "SET VARIABLE ", variablename, " ", value, "\n");
	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 1;

}

/*
 * stream_file
 *	Send the given file, allowing playback to be interrupted by the given
 *	digits, if any. Use double quotes for the digits if you wish none to be
 *	permitted. If sample offset if provided then the audio will seek to
 *	sample offset before play starts. Returns 0 if playback completes
 *	without a digit being pressed, or the ASCII numerical value of the
 *	digit if one was pressed, or -1 on error or if the channel was
 *	disconnected. Remember, the file extension must not be included in the
 *	filename.
 * link
 *	http://www.voip-info.org/wiki/view/stream+file
 * params (required, optional, optional)
 *	<file> [<escape_digits>] [<sample_offset>]
 * returns
 *	Success: A two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = "0"
 *		char *data[2] = "endpos=<offset>"
 *		Digit pressed:
 *			char *data[0] = "200"
 *			char *data[1] = "<digit>"
 *			char *data[2] = "endpos=<offset>"
 *	Failure: A two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = "-1"
 *		char *data[2] = "endpos=<sample_offset>"
 *		Failure on open:
 *			char *data[0] = "200"
 *			char *data[1] = "0"
 *			char *data[2] = "endpos=0"
 */
char ** stream_file(const char *file, const char *escape_digits, const char
							*sample_offset) {

	char **data, *cmd;

	/*
	 * If <file> is not specified by the user, quit early to save
	 * processing time.
	 */
	if (strcmp(file, "") == 0) {
		print_debug("ERROR! <file> must not be empty.");
		data = create_dummy("200", "0", "endpos=0");
		return data;
	}

	/*
	 * If <sample_offset> is not defined, then just use escape_digits.
	 * Otherwise, <sample_offset> is defined, so use it.
	 */
	if (strcmp(sample_offset, "") == 0) {
		if (strcmp(escape_digits, "") == 0) {
			cmd = format_str(3, "STREAM FILE ", file, " \"\"\n");
		} else {
			cmd = format_str(5, "STREAM FILE ", file, " ",
							escape_digits, "\n");
		}
	} else {
		if (strcmp(escape_digits, "") == 0) {
			cmd = format_str(5, "STREAM FILE ", file, " \"\" ",
							sample_offset, "\n");
		} else {
			cmd = format_str(5, "STREAM FILE ", file, " ",
							escape_digits, "\n");
		}
	}

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * control_stream_file
 *	Send the given file, allowing playback to be controlled by the given
 *	digits, if any. Use double quotes for the digits if you wish none to be
 *	permitted. Returns 0 if playback completes without a digit being
 *	pressed, or the ASCII numerical value of the digit if one was pressed,
 *	or -1 on error or if the channel was disconnected. Remember, the file
 *	extension must not be included in the filename.
 * link
 *	http://www.voip-info.org/wiki/view/control+stream+file
 * params (required, optional, optional, optional, optional, optional)
 *	<file> [<escape_digits>] [<skipms>] [<ffchar>] [<rewchr>] [<pausechr>]
 * returns
 *	Success: A two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = "0"
 *		char *data[2] = "endpos=<offset>"
 *		Digit pressed:
 *			char *data[0] = "200"
 *			char *data[1] = "<digit>"
 *			char *data[2] = "endpos=<offset>"
 *	Failure: A two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = "-1"
 *		char *data[2] = "endpos=<offset>"
 *		Failure on open:
 *			char *data[0] = "200"
 *			char *data[1] = "0"
 *			char *data[2] = "endpos=0"
 * NOTE: <ffchar> and <rewchar> default to * and # respectively.
 */
char ** control_stream_file(const char *file, const char *escape_digits, const
	char *skipms, const char *ffchar, const char *rewchr, const char
								*pausechr) {

	int given = 0;
	char **data, *cmd;

	/*
	 * If <file> is not specified by the user, quit to save processing
	 * time.
	 */
	if (strcmp(file, "") == 0) {
		print_debug("ERROR! <file> must not be empty.");
		data = create_dummy("200", "0", "endpos=0");
		return data;
	}

	if (strcmp(escape_digits, "") != 0)
		given = 1;

	if (strcmp(skipms, "") == 0) {
		cmd = format_str(5, "CONTROL STREAM FILE ", file, " ",
				(given ? escape_digits : "\"\""), "\n");
	} else if (strcmp(ffchar, "") == 0) {
		cmd = format_str(7, "CONTROL STREAM FILE ", file, " ",
			(given ? escape_digits : "\"\""), " ", skipms, "\n");
	} else if (strcmp(rewchr, "") == 0) {
		cmd = format_str(9, "CONTROL STREAM FILE ", file, " ",
			(given ? escape_digits : "\"\""), " ", skipms, " ",
								ffchar, "\n");
	} else if (strcmp(pausechr, "") == 0) {
		cmd = format_str(11, "CONTROL STREAM FILE ", file, " ",
			(given ? escape_digits : "\"\""), " ", skipms, " ",
						ffchar, " ", rewchr, "\n");
	} else {
		cmd = format_str(13, "CONTROL STREAM FILE ", file, " ",
			(given ? escape_digits : "\"\""), " ", skipms, " ",
				ffchar, " ", rewchr, " ", pausechr, "\n");
	}

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * tdd_mode
 *	Enable/disable TDD transmission/reception on a channel. Returns 1 if
 *	successful, or 0 if channel is not TDD-capable.
 * link
 *	http://www.voip-info.org/wiki/view/tdd+mode
 * params (required)
 *	<on|off|mate>
 * returns
 *	Success: 1
 *	Failure: -1 OR 0
 */
int tdd_mode(const char *toggle) {

	int status;
	char **data, *cmd;

	/*
	 * If <toggle> isn't specified by the user, quit early to save
	 * processing power.
	 */
	if (strcmp(toggle, "") == 0) {
		print_debug("ERROR! <toggle> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "TDD MODE ", toggle, "\n");
	data = evaluate(cmd);
	free(cmd);

	if (strcmp(data[1], "1") == 0)
		status = 1;
	else if (strcmp(data[1], "0") == 0)
		status = 0;
	else
		status = -1;

	free_2d_array(data);
	return status;

}

/*
 * verbose
 *	Sends <message> to the console via verbose message system.
 * link
 *	http://www.voip-info.org/wiki/view/verbose
 * params (required, optional)
 *	<message> <level>
 * returns
 *	Success: 1
 *	Failure: Never fails. :)
 */
int verbose(const char *message, const char *level) {

	char **data, *cmd;

	/*
	 * If the parameters weren't specified by the user, quit early and save
	 * processing time.
	 */
	if (strcmp(message, "") == 0) {
		print_debug("ERROR! <message> must not be empty.");
		return 1;
	}

	if (strcmp(level, "") == 0)
		cmd = format_str(3, "VERBOSE \"", message, "\"\n");
	else
		cmd = format_str(5, "VERBOSE \"", message, "\" ", level, "\n");

	data = evaluate(cmd);
	free(cmd);

	free_2d_array(data);
	return 1;

}

/*
 * wait_for_digit
 *	Waits up to <timeout> milliseconds for channel to receive a DTMF digit.
 *	Returns -1 on channel failure, 0 if no digits are received in the
 *	<timeout>, or the numerical value of the ASCII of the digit if one is
 *	received. Use -1 for the timeout value if you desire the call to block
 *	indefinitely.
 * link
 *	http://www.voip-info.org/wiki/view/wait+for+digit
 * params (required)
 *	<timeout>
 * returns
 *	Success: ASCII value of the digit pressed.
 *	Failure: -1
 *		Timeout: 0
 */
int wait_for_digit(const char *timeout) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify <timeout>, return a failure quickly.
	 */
	if (strcmp(timeout, "") == 0) {
		print_debug("ERROR! <timeout> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "WAIT FOR DIGIT ", timeout, "\n");
	data = evaluate(cmd);

	if (strcmp(data[1], "0") == 0)
		status = 0;
	else if (strcmp(data[1], "-1") == 0)
		status = -1;
	else
		status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech create
 *	Create a speech object to be used by the other Speech AGI commands.
 * link
 *	none
 * params (required)
 *	<engine>
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_create(const char *engine) {

	int status;
	char **data, *cmd;

	/*
	 * If <engine> isn't specified, exit quickly.
	 */
	if (strcmp(engine, "") == 0) {
		print_debug("ERROR! <engine> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "SPEECH CREATE ", engine, "\n");
	data = evaluate(cmd);
	free(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_set
 *	Set an engine-specific setting.
 * link
 *	none
 * params (required, required)
 *	<name> <value>
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_set(const char *name, const char *value) {

	int status;
	char **data, *cmd;

	/*
	 * If <name> or <value> isn't specified, exit quickly.
	 */
	if (strcmp(name, "") == 0) {
		print_debug("ERROR! <name> must not be empty.");
		return -1;
	} else if (strcmp(value, "") == 0) {
		print_debug("ERROR! <value> must not be empty.");
		return -1;
	}

	cmd = format_str(5, "SPEECH SET ", name, " ", value, "\n");
	data = evaluate(cmd);
	free(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_destroy
 *	Destroy the speech object created by SPEECH CREATE.
 * link
 *	none
 * params
 *	none
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: Whatever is inside the result field.
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_destroy(void) {

	int status;
	char **data;

	data = evaluate("SPEECH DESTROY\n");
	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_load_grammar
 *	Loads the specified grammar as the specified name.
 * link
 *	none
 * params (required, required)
 *	<name> <path>
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_load_grammar(const char *name, const char *path) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify the required parameters, exit quickly.
	 */
	if (strcmp(name, "") == 0) {
		print_debug("ERROR! <name> must not be empty.");
		return -1;
	} else if (strcmp(path, "") == 0) {
		print_debug("ERROR! <path> must not be empty.");
		return -1;
	}

	cmd = format_str(5, "SPEECH LOAD GRAMMAR ", name, " ", path, "\n");
	data = evaluate(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_unload_grammar
 *	Unloads the specified grammar.
 * link
 *	none
 * params (required)
 *	<name>
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_unload_grammar(const char *name) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify <name>, exit quickly.
	 */
	if (strcmp(name, "") == 0) {
		print_debug("ERROR! <name> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "SPEECH UNLOAD GRAMMAR ", name, "\n");
	data = evaluate(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_activate_grammar
 *	Activates the specified grammar on the speech object.
 * link
 *	none
 * params (required)
 *	<name>
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_activate_grammar(const char *name) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify <name>, exit quickly.
	 */
	if (strcmp(name, "") == 0) {
		print_debug("ERROR! <name> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "SPEECH ACTIVATE GRAMMAR ", name, "\n");
	data = evaluate(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_deactivate_grammar
 *	Deactivates the specified grammar on the speech object.
 * link
 *	none
 * params (required)
 *	<name>
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
int speech_deactivate_grammar(const char *name) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify <name>, exit quickly.
	 */
	if (strcmp(name, "") == 0) {
		print_debug("ERROR! <name> must not be empty.");
		return -1;
	}

	cmd = format_str(3, "SPEECH DEACTIVATE GRAMMAR ", name, "\n");
	data = evaluate(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}

/*
 * speech_recognize
 *	Plays back given prompt while listening for speech and DTMF.
 * link
 *	none
 * params (required, required, optional)
 *	<prompt> [<timeout>] [<offset>]
 * returns
 *	Success: A two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = ??
 *		char *data[2] = ??
 *	Failure: A two-dimensional array of values. The values are:
 *		char *data[0] = "200"
 *		char *data[1] = "-1"
 *		char *data[2] = ""
 * NOTE: This command doesn't appear to be fully implemented by asterisk. DO
 *	NOT USE IT!
 */
char ** speech_recognize(const char *prompt, const char *timeout, const char
								*offset) {

	int given = 0;
	char **data, *cmd;

	/*
	 * If any of the required parameters weren't specified. Quit quickly.
	 */
	if (strcmp(prompt, "") == 0) {
		print_debug("ERROR! <prompt> must be specified.");
		data = create_dummy("200", "-1", "");
		return data;
	}

	if (strcmp(timeout, "") != 0)
		given = 1;

	if (strcmp(offset, "") == 0) {
		cmd = format_str(5, "SPEECH RECOGNIZE ", prompt, " ",
				(given ? timeout : _DEFAULT_TIMEOUT), "\n");
	} else {
		cmd = format_str(7, "SPEECH RECOGNIZE ", prompt, " ",
			(given ? timeout : _DEFAULT_TIMEOUT), " ", offset,
									"\n");
	}

	data = evaluate(cmd);
	free(cmd);

	return data;

}

/*
 * gosub
 *	Cause the channel to execute the specified dialplan subroutine,
 *	returning to the dialplan with execution of a return().
 * link
 *	none
 * params (required, required, required, optional)
 *	<context> <extension> <priority> [<arguments>]
 * returns
 *	Success: Whatever is inside the result field.
 *	Failure: -1
 * NOTE: This command has NOT YET BEEN IMPLEMENTED by asterisk. DO NOT USE IT!
 */
int gosub(const char *context, const char *extension, const char *priority,
						const char *arguments) {

	int status;
	char **data, *cmd;

	/*
	 * If the user didn't specify the required arguments, exit quickly.
	 */
	if (strcmp(context, "") == 0) {
		print_debug("ERROR! <context> must not be empty.");
		return -1;
	} else if (strcmp(extension, "") == 0) {
		print_debug("ERROR! <extension> must not be empty.");
		return -1;
	} else if (strcmp(priority, "") == 0) {
		print_debug("ERROR! <priority> must not be empty.");
		return -1;
	}

	if (strcmp(arguments, "") == 0) {
		cmd = format_str(7, "GOSUB ", context, " ", extension, " ",
							priority, "\n");
	} else {
		cmd = format_str(9, "GOSUB ", context, " ", extension, " ",
					priority, " \"", arguments, "\n");
	}

	data = evaluate(cmd);
	free(cmd);

	status = atoi(data[1]);

	free_2d_array(data);
	return status;

}
