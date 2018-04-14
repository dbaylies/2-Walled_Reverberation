#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for malloc() and exit()
#include <sndfile.h>	/* libsndfile */
#include "room_acoustics.h"

// This code only works on single-channel signals

// function prototype
void usage(void);

int main(int argc, char *argv[]) {

    char *txt = ".txt", *wav = ".wav";
    char *fileext[3];
    char *paramfile, *ifile, *ofile;
    int len[3];
    int i, j, k, l, m;
    int fs, max_delay, extra_frames, tot_blocks, tot_in_blocks, extra_in_frames, to_read, count;
    Pt source, image[NUM_IMAGE];
    Wall wall[NUM_WALL];
    Path path[NUM_PATH];
    /* libsndfile data structures */
	SNDFILE *isfile, *osfile; 
	SF_INFO isfinfo, osfinfo;
    // **MAY PERHAPS BE NECESSARY TO ALLOCATE ALL BUFFERS INSTEAD**
    float *max_delay_block, ibuf[BLOCK_LEN], obuf[BLOCK_LEN];

    /* zero libsndfile structures */
	memset(&isfinfo, 0, sizeof(isfinfo));
  	memset(&osfinfo, 0, sizeof(osfinfo));

    /* Check for correct number of arguments */
    if (argc != 4) {
        puts("Error: Incorrect number of input arguments.");
        usage();
    }

    /* Check for correct file extensions */
    // Loop over input arguments
    for (i = 0; i < 3; i++) {
        len[i] = strlen(argv[i+1]);
        // Quit if length of argument is less than 4
        if (len[i] < 4) {
            puts("Error: Improper arguments.");
            usage();
        }
        // Set up string holder
        fileext[i] = malloc(5);
        fileext[i][4] = '\0';

        // Store file extension in fileext array
        for (j = 0; j < 4; j++)
            fileext[i][j] = argv[i+1][len[i]-(4-j)];
    }
    // Check if extensions are correct
    if (strcmp(txt,fileext[0])) {
        puts("Error: First argument must be a .txt file.");
        usage();
    }

    if (strcmp(wav,fileext[1]) || strcmp(wav, fileext[2])) {
        puts("Error: Second and third arguments must be .wav files.");
        usage();
    }

    paramfile = argv[1];
    ifile = argv[2];
    ofile = argv[3];

    // Call parser
    parse_param_file(paramfile, &source, wall);

    // Open input file
    if ((isfile = sf_open(ifile, SFM_READ, &isfinfo)) == NULL) {
		printf("Error opening %s\n", ifile);
		return 0;
	}

    /* Print input file information */
	printf("\nInput audio file %s:\n\tFrames: %d Channels: %d Samplerate: %d\n", 
		ifile, (int)isfinfo.frames, isfinfo.channels, isfinfo.samplerate);
    // Set output file info
    osfinfo.samplerate = isfinfo.samplerate;
	osfinfo.channels = isfinfo.channels;
	osfinfo.format = isfinfo.format;
	/* open output file  for writing*/
    if ((osfile = sf_open(ofile, SFM_WRITE, &osfinfo)) == NULL) {
        printf("Error opening %s\n", ofile);
        return 0;
    }
    // Store sampling rate
    fs = isfinfo.samplerate;

    max_delay = calculate_paths(&source, wall, path, image, fs);
    
    // Allocate and initialize buffers
    max_delay_block = (float *)malloc(sizeof(float)*(max_delay+BLOCK_LEN));
    for (i = 0; i < max_delay+BLOCK_LEN; i++) {
        max_delay_block[i] = 0;
    }
        
    // Determine total number of blocks to loop over, rounded down
    // Do this for both input file length and output file length
    // account for partial block at the end in both cases
    tot_in_blocks = (isfinfo.frames)/1024 + 1;
    extra_in_frames = (isfinfo.frames)%1024;

    //Subtract extra_in_frames in order to divide by 1024 properly - add this frame back in at end, as well as extra frame from excess of max_delay
    tot_blocks = (isfinfo.frames+max_delay-extra_in_frames)/1024 + 1 + 1;
    extra_frames = (max_delay)%1024;
    //printf("tot blocks is: %f\n",tot_blocks);
    
    for (k = 0; k < tot_blocks; k++) {
        //printf("block num: %i\n", k);
        if (k == (tot_in_blocks-1)) {
            to_read = extra_in_frames;
        } else if (k == (tot_blocks-1)) {
            to_read = extra_frames;
        } else if ((k < (tot_blocks-1)) && (tot_blocks != tot_in_blocks)) {
            to_read = BLOCK_LEN;
        } else {
            puts("Something happened\n");
            exit(0);
        }

        if (k < tot_in_blocks) {
            // Read in next block
            // ** DOES ISFILE POINTER AUTOMATICALLY ADVANCE?
            if ( (count = sf_read_float (isfile, ibuf, to_read)) != to_read) {
                fprintf(stderr, "sfread error: on block %d\n", k);
                exit(0);
            }
            // Copy ibuf into end of max delay block
            for (i = 0; i<to_read; i++) {
                max_delay_block[i+max_delay] = ibuf[i];
            }
        } else {
            // Copy zeros into end of max delay block after all of input file is read
            for (i = 0; i<to_read; i++) {
                max_delay_block[i+max_delay] = 0;
            }
        }
        //if (k==202) printf("Frames read: %i\n",to_read);

        // Calculate obuf
        for (m = 0; m < to_read; m++) {
            obuf[m] = 0;
            for (l = 0; l < NUM_PATH; l++) {
                obuf[m] += max_delay_block[m + max_delay - path[l].delay]*path[l].atten;
            }
            // if (k == 202) 
                // printf("%f\n",obuf[m]);        
        }

        // Write obuf
        // **SAME QUESTION HERE
        if ( (count = sf_write_float (osfile, obuf, to_read)) != to_read) {
			fprintf(stderr, "sfwrite error: on block %d\n", k);
			exit(0);
		}
        // Prepare for next loop
        for (i = 0; i < max_delay; i++) {
            max_delay_block[i] = max_delay_block[i+to_read];
        }
    }

    /* Must close file; output will not be written correctly if you do not do this */
	sf_close (isfile);
	sf_close (osfile);

    // Open output file again
    if ((osfile = sf_open(ofile, SFM_READ, &osfinfo)) == NULL) {
		printf("Error opening %s again\n", ofile);
		return 0;
	}
    printf("Samples in isfile: %zu, Samples in osfile: %zu\n", isfinfo.frames, osfinfo.frames);
    sf_close (osfile);

	/* free all buffer storage */
	printf("Freeing buffers\n");
    if (max_delay_block != NULL)
        free(max_delay_block);

    return 0;
}

// usage function
void usage(void) {
    puts("Correct format is: ./room_acoustics parameters.txt ifile.wav ofile.wav");
    exit(0);
}