#include <stdio.h>
#include <string.h>

#include "util/config/config.h"
#include "event/event.h"
#include "util/log/log.h"

#define QUOTE_DEFINE_(x) #x
#define QUOTE_DEFINE(x) QUOTE_DEFINE_(x)
#define DEFAULT_CONFIG "vectir.conf"

void 
create_default_config() {
}

void 
load_config() {
	char path[512];
	char last_char;
	unsigned int is_path = 0;
	int config_result;
	int handle2;
	
#ifdef CONFIG_LOCATION
	sprintf(path, "%s", QUOTE_DEFINE(CONFIG_LOCATION));
	
	is_path = (strchr(path, '/') == NULL);
	
	// Get last character
	last_char = path[strlen(path)-1];
	
	// Was the location a full path and file name?
	if (last_char == '/') {
		LOG_DEBUG("Location specified but does not include a "
			"file name. Using default '%s'", DEFAULT_CONFIG);
		
		sprintf(path, "%s%s", path, DEFAULT_CONFIG);
	} else if (!is_path) {
		LOG_DEBUG("Location is a full path and file name.");
	} else {
		LOG_DEBUG("Location is only a file name.");
	}	
#else
	LOG_DEBUG("No config location specified, using default.");
	sprintf(path, DEFAULT_CONFIG);
#endif
	
	// Attempt to load the config.
	// If the file is missing from the location specified, we force generation
	// of one with default values.
	// If the file was found, but contains incorrect settings, we terminate.
	log_write(LOG_LEVEL_INFO, "Reading config file (%s)...", path);
	config_result = config_load(path);
	if (config_result == CONFIG_NOT_FOUND) {
		log_write(LOG_LEVEL_ERROR, "Config file not found. " \
			"Generating one with default values");
		create_default_config();
	} else if (config_result == CONFIG_INVALID) {
		log_write(LOG_LEVEL_SEVERE, "Config file is invalid");
		return;
	}
	
	if (config_result > 0) {
		log_write(LOG_LEVEL_INFO, "Config file successfully loaded");
	}
}

void test_event(unsigned int size, char *data) {
	LOG_DEBUG("Test event called");
}

int 
main(int argc, char **argv) {
	Config config;
	
	// init basic logging to stdout for errors and severe failures
	log_init(LOG_TO_STDOUT | LOG_TO_FILE, "log.txt", 
	#ifdef DEBUG
		LOG_LEVEL_DEBUG |
	#endif
		LOG_LEVEL_INFO | LOG_LEVEL_ERROR | LOG_LEVEL_SEVERE);
		
	LOG_DEBUG("Logging initialised...");	
	
	load_config();
	
	event_init();
	
	event_subscribe(1, (ptrEventCallback)&test_event);
	event_close();

	return 0;
}
