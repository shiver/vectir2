#ifndef LOG_H
#define LOG_H

#define LOG_TO_STDOUT		1
#define LOG_TO_FILE			2

#define LOG_LEVEL_INFO		1
#define LOG_LEVEL_WARN 		2
#define LOG_LEVEL_DEBUG		4
#define LOG_LEVEL_ERROR		8
#define LOG_LEVEL_SEVERE	16

#define LOG_DEBUG(...) 		log_write(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) 		log_write(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_ERROR(...) 		log_write(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_WARN(...) 		log_write(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_SEVERE(...) 	log_write(LOG_LEVEL_SEVERE, __VA_ARGS__)

/*
 * Initialising logging.
 * options takes one or many LOG_TO_x flags.
 * filename is required if LOG_TO_FILE is flagged in output_options
 * levels takes one or many LOG_LEVEL_x flags and determines which messages
 * are send to the output queue.
 * Returns true if initialisation succeeds.
 */
unsigned int log_init(unsigned char options, char *filename, 
						unsigned char levels);

/*
 * Ensures all logging related resources are freed and file descriptors are 
 * closed.
 * Note: If you called log_init() you should make sure that you call log_close()
 */
void log_close();
						
/*
 * Write out to the log.
 * level takes a single LOG_LEVEL_x and indicates the type of log that is being
 * 		written to.
 * fmt takes a format similar to that in printf or scanf with the varadic
 *		arguments corresponding to each token in the fmt.
 */
void log_write(unsigned char level, const char *fmt, ...);

/*
 * Used to adjust log levels after logging has already been initialised.
 * new_levels accepts one or many LOG_LEVEL_x flags.
 */
void log_set_levels(unsigned char new_levels);

/*
 * Used to adjust output options after logging has already been initialised.
 * new_options accepts one or many LOG_TO_x flags.
 */
void log_set_output_options(unsigned char new_options);

#endif
