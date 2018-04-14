gcc -std=c99 -Wall \
	plot_room.c parse_param_file.c calculate_paths.c \
	-o plot_room -I/usr/local/include/plplot -L/usr/local/lib -lplplot
