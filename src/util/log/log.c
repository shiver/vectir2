#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static unsigned char log_levels;
static unsigned char output_options;
static FILE *fp;

/*
 * Opens a log file for writing
 */
static
void
open_file(const char *filename) {
	fp = fopen(filename, "w");
	
	if (fp == NULL) {
		printf("Could not open log file for writing (%s).\n" \
			"Logs will only be sent to STDOUT.", filename);
	}
}

static
void
close_file() {
	if (fp == NULL) {
		log_write(LOG_LEVEL_WARN, "Log file could not be closed. File is not " \
			"open.");
		return;
	}
	
	fclose(fp);
}

unsigned int 
log_init(unsigned char options, char *filename, 
			unsigned char levels) {
	log_levels = levels;
	output_options = options;
	
	if ((output_options & LOG_TO_FILE) == LOG_TO_FILE) {
		open_file(filename);
	}
	
	return 1;
}

void 
log_write(unsigned char level, const char *fmt, ...) {
	char str[1024];
	char level_str[7];
	char *s_arg;
	int i_arg;
	char c;
	va_list args;
	
	// Only continue if the log level for this write is one we are watching
	if ((log_levels & level) != level) {
		return;
	}
	
	if (level == LOG_LEVEL_INFO) {
		sprintf(level_str, "INFO");
	} else if(level == LOG_LEVEL_WARN) {
		sprintf(level_str, "WARN");
	} else if(level == LOG_LEVEL_DEBUG) {
		sprintf(level_str, "DEBUG");
	} else if(level == LOG_LEVEL_ERROR) {
		sprintf(level_str, "ERROR");
	} else if(level == LOG_LEVEL_SEVERE) {
		sprintf(level_str, "SEVERE");
	}
	
	sprintf(str, "[%s] ", level_str);
	
	// deal with the variable arguments
	va_start(args, fmt);
	
	while(*fmt) {
		if ((c = *fmt++) == '%') {
			switch(*fmt) {
				case 's':	// string
					s_arg = va_arg(args, char *);
					strcat(str, s_arg);
					fmt++;
					break;
				case 'd':	// string
					i_arg = va_arg(args, int);
					sprintf(str, "%s%d", str, i_arg);
					fmt++;
					break;
			}
		} else {
			sprintf(str, "%s%c", str, c);
		}
	}
	
	va_end(args);
	
	// Log to the console
	if ((output_options & LOG_TO_STDOUT) == LOG_TO_STDOUT) {
		printf("%s\n", str);
	}
	
	if ((output_options & LOG_TO_FILE) == LOG_TO_FILE) {
		fprintf(fp, "%s\n", str);
	}
}

void 
log_set_levels(unsigned char new_levels) {
	log_levels = new_levels;
}

void
log_set_output_options(unsigned char new_options) {
	output_options = new_options;
}

void log_close() {
	close_file();
}

