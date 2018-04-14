gcc -std=c99 -Wall \
	room_acoustics.c parse_param_file.c calculate_paths.c \
	-o room_acoustics -I/usr/local/include -L/usr/local/lib -lsndfile
