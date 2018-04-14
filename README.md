# 2-Walled_Reverberation

Simulates reverberation from a sound source and two walls using a mirror-image method. In total, four paths are calculated and their delays used to simulate the reverberation for a 2-walled environment. libsndfile is used to read and write wav files.

This code was created as part of an assignment in NYU's C Programming for Music Technology course, taught by Schuyler Quackenbush of the MPEG group.

"build1", "calculate_paths", "paramtersX", and "parse_param_file", were all written and provided by Professor Quackenbush. The core of the project, "room_acoustics", was written by me.

Usage for the room_acoustics executable (after running the build script) is "./room_acoustics parameters.txt ifile.wav ofile.wav". For ifile.wav, you can start by using signals/svega.wav, for example. ofile.wav will be written to the working directory.
