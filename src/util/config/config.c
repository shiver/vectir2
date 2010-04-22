#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "../log/log.h"
#include "../misc/stringutils.h"

static int last_handle = 0;
static Config *configs = NULL;

/*
 * Retrieves a pointer to the last Config struct in the linked list
 */
static
Config *
get_last_config() {
	Config *config;
	
	// cycle through all the configs until we come to an empty slot
	config = configs;
	while (config->next_config != NULL) {
		config = (Config *)config->next_config;
	}
	
	return config;
}

/*
 * Retrieves a pointer to the Config struct directly before the one specified.
 * Returns NULL if not found or the config is the first in the list
 */
static
Config *
get_config_before(Config *config) {
	Config *fconfig;	// will hold the found config
	
	if (configs == NULL) {
		log_write(LOG_LEVEL_ERROR, "No Configs found");
		return NULL;
	}
	
	if (configs == config) {
		log_write(LOG_LEVEL_DEBUG, "Config found is first in the list");
		return NULL;
	}
	
	fconfig = configs;
	while (fconfig->next_config != NULL) {
		if (fconfig->next_config == config) {
			return fconfig;
		}
		
		fconfig = fconfig->next_config;
	}
	
	return NULL;
}

/* 
 * Retrieves a pointer to the last pair for the supplied Config struct
 * If no pairs have been created yet we return NULL.
 */
static
Pair *
get_last_pair(Config *config) {
	Pair *pair;
	
	pair = config->first_pair;
	if (pair == NULL) {
		return NULL;
	}
	
	// cycle through all the pairs until we come to an empty slot
	while (pair->next_pair != NULL) {
		pair = pair->next_pair;
	}
	
	return pair;
}

/*
 * Retrieves a pointer to a Config struct with the specified handle
 */
static
Config *
get_config(int handle) {
	Config *config;
	if (configs == NULL) {
		return NULL;
	}
	
	config = configs;
	if (config->handle == handle) {
		return config;
	}
	
	while(config->next_config != NULL) {
		config = config->next_config;
		if (config->handle == handle) {
			return config;
		}
	}
	
	return NULL;
}

int 
config_set(int handle, char *key, char *value) {
	Config *config;
	Pair *pair;
	
	config = get_config(handle);
	if (config == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable to set pair. " \
			"Config handle %d not found", handle);
		return CONFIG_INVALID_HANDLE;
	}
	
	// get the last used pair in the config. NULL indicates that we need to
	// allocate for the first pair in the list.
	pair = get_last_pair(config);
	if (pair == NULL) {
		log_write(LOG_LEVEL_DEBUG, "No pairs have been created yet. " \
			"Starting from the first");
		pair = malloc(sizeof(Pair));
		memset(pair, '\0', sizeof(Pair));
		config->first_pair = pair;
	} else {
		pair->next_pair = malloc(sizeof(Pair));
		memset(pair->next_pair, '\0', sizeof(Pair));
		pair = pair->next_pair;
	}
	
	if (pair == NULL) {
		log_write(LOG_LEVEL_ERROR, "Failed to allocate mem for pair");
		log_write(LOG_LEVEL_DEBUG, "handle=%d, key=%s, value=%s", handle, 
			key, value);
		return CONFIG_FAILED;
	}
	
	pair->key = malloc(strlen(key) + 1);
	strcpy(pair->key, key);
	pair->value = malloc(strlen(value) + 1);
	strcpy(pair->value, value);
	
	log_write(LOG_LEVEL_DEBUG, "Pair set %s = %s", key, value);
	
	return CONFIG_SUCCESS;
}

char *
config_get(int handle, char *key) {
	Config *config;
	Pair *pair;
	
	config = get_config(handle);
	if (config == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable to get value. " \
			"Config handle %d not found", handle);
		return NULL;
	}

	if (config->first_pair == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable to get value with key '%s'. " \
			"No pairs defined", key);
	}
	
	pair = config->first_pair;
	// check the first pair
	if (strcmp(pair->key, key) == 0) {
		return pair->value;
	}
	
	// check all subsequent pairs
	while (pair->next_pair != NULL) {
		pair = pair->next_pair;
		if (strcmp(pair->key, key) == 0) {
			return pair->value;
		}
	}
	
	log_write(LOG_LEVEL_ERROR, "No pair found for key '%s'", key);
	return NULL;
}

/*
 * Opens a config file for processing.
 * handle as returned by a call to config_create()
 * Note: Expects the Config struct to already have the file name populated.
 */
static
int 
open(int handle) {
	FILE *fp;
	Config *config;
	int line;
	char str[255];
	char key[50], value[50];
	int result = CONFIG_SUCCESS;
	
	config = get_config(handle);
	if (config == NULL) {
		log_write(LOG_LEVEL_DEBUG, "Config struct not found: %d. " \
			"Has it been created yet? config_create()", handle);
		return ;
	}
	
	fp = fopen(config->filename, "r");
	
	if (fp == NULL) {
		return CONFIG_NOT_FOUND;
	}
	
	line = 1;
	while (!feof(fp)) {
		str[0] = '\0';
		fgets(str, sizeof(str), fp);
		if (strlen(trim_whitespace(str)) > 0) {
			if (sscanf(str, "%s %s", key, value) != 2) {
				log_write(LOG_LEVEL_ERROR, "Malformed config file at line %d:\n"
							"\t%s", line, trim_whitespace(str));
							
				result = CONFIG_INVALID;
				break;
			} else {
				log_write(LOG_LEVEL_DEBUG, "Found pair at line %d: %s = %s", 
					line, key, value);
				if (config_set(handle, key, value) != CONFIG_SUCCESS) {
					log_write(LOG_LEVEL_SEVERE, "Failed to set config for " \
						"key: %s", key);
					result = CONFIG_INVALID;
					break;
				}
			}
		}
				
		line++;
	}
	
	fclose(fp);
	
	return result;
}

void 
config_set_filename(int handle, char *filename) {
	Config *config;
	
	config = get_config(handle);
	if (config != NULL) {
		config->filename = malloc(strlen(filename) + 1);
		strcpy(config->filename, filename);
	}
}

int 
config_load(char *filename) {
	int handle;
	int result;
	
	// setup an empty config
	handle = config_create();
	config_set_filename(handle, filename);
	result = open(handle);
	
	if (result == CONFIG_SUCCESS) {
		return handle;
	} else {
		return result;
	}
}

int 
config_create() {
	Config *new_config = NULL;
	Config *last_config;
	
	new_config = malloc(sizeof(Config));
	memset(new_config, '\0', sizeof(Config));
	
	new_config->handle = ++last_handle;
	
	if (configs == NULL) {
		configs = new_config;
	} else {
		last_config = get_last_config();
		last_config->next_config = new_config;
	}
	
	return new_config->handle;
}

int
config_close(int handle) {
	Config *config;
	Pair *pair;
	Pair *next_pair;
	int pair_count = 0;

	log_write(LOG_LEVEL_DEBUG, "Closing config %d...", handle);
	config = get_config(handle);
	if (config == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable close config. " \
			"Config handle %d not found", handle);
		return CONFIG_INVALID_HANDLE;
	}
	
	// free all pairs' resources
	if (config->first_pair != NULL) {
		pair = config->first_pair;
		
		// free starting pair
		free(pair->key);
		free(pair->value);
		free(pair);
		
		pair_count++;
		
		// free remainging pairs
		while(pair->next_pair != NULL) {
			pair = pair->next_pair;
			
			free(pair->key);
			free(pair->value);
			free(pair);
			pair_count++;
		}
		
		config->first_pair = NULL;
	}
	
	free(config->filename);
	
	// If this config is the first, we need to get the next in the list and 
	// move it to the front.
	if (configs == config && config->next_config != NULL) {
		log_write(LOG_LEVEL_DEBUG, "Config was first in list. Shifting " \
			"remaining configs up one slot.");
		configs = config->next_config;
	} else {
		// fill in the link between configs
		get_config_before(config)->next_config == config->next_config;
	}
	
	free(config);
	
	log_write(LOG_LEVEL_DEBUG, "Config closed (%d pairs freed)", pair_count);
}

int
config_save(int handle) {
	Config *config;
	Pair *pair;
	FILE *fp;

	log_write(LOG_LEVEL_DEBUG, "Saving config %d...", handle);
	config = get_config(handle);
	if (config == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable to save config. " \
			"Config handle %d not found", handle);
		return CONFIG_INVALID_HANDLE;
	}
	
	if (config->filename == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable to save config. " \
			"File name has not been set");
		return CONFIG_FAILED;
	}
	
	if (config->first_pair == NULL) {
		log_write(LOG_LEVEL_INFO, "Nothing to write to config. " \
			"No pairs found");
		return CONFIG_SUCCESS;
	}
	
	// overwrites file if it exists
	fp = fopen(config->filename, "w");
	
	if (fp == NULL) {
		log_write(LOG_LEVEL_ERROR, "Unable to save config. " \
			"File could not be opened for writing (%s)", config->filename);
		return CONFIG_FAILED;
	}
	
	// write pair data to file
	pair = config->first_pair;
	fprintf(fp, "%s %s\n", pair->key, pair->value);
	while (pair->next_pair != NULL) {
		pair = pair->next_pair;
		fprintf(fp, "%s %s\n", pair->key, pair->value);
	}
	
	fclose(fp);
	
	return CONFIG_SUCCESS;
}


