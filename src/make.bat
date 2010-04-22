mingw32-gcc -DDEBUG -DCONFIG_LOCATION=vectir.conf -o vectir.exe ^
	main.c util/log/log.c ^
	util/config/config.c ^
	util/misc/stringutils.c ^
	util/misc/queue.c ^
	event/event.c
	