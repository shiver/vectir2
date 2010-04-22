#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_SUCCESS			1

#define CONFIG_FAILED			-1
#define CONFIG_NOT_FOUND		-2
#define CONFIG_INVALID			-3
#define CONFIG_STRUCT_MISSING 	-4

#define CONFIG_INVALID_HANDLE	-5

struct sPair {
	char *key;
	char *value;
	struct sPair *next_pair;
};

struct sConfig {
	int handle;
	char *filename;
	struct sPair *first_pair;
	struct sConfig *next_config;
};

typedef struct sConfig Config;
typedef struct sPair Pair;

/*
 * Loads a config file and returns the unique handle to that config.
 * filename takes the full path to the config file
 * Possible error return codes are:
 *		CONFIG_NOT_FOUND
 * 		CONFIG_INVALID
 */
int config_load(char *filename);

/*
 * Frees resources associated with the specified Config handle.
 * All configs should be explicitly closed once they are no longer required.
 */
int config_close(int handle);

/*
 * Retrieves the pair value for the specified Config struct based on the
 * supplied key.
 * Returns a null terminated string containing the value.
 */
char *config_get(int handle, char *key);

/*
 * Stores a key value pair and associates it with the specified Config struct
 * Returns CONFIG_SUCCESS if the call is successful
 * Possible error return codes are:
 * 		CONFIG_INVALID_HANDLE
 * 		CONFIG_FAILED
 */
int config_set(int handle, char *key, char *value);

/*
 * config_create initialises an empty Config structure.
 * Successful initiliasation results in a positive handle being returned.
 * 		This handle is used in subsequent config requests.
 */
int config_create();

void config_set_filename(int handle, char *filename);

int config_save(int handle);

#endif
