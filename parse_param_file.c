#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>     /* strtok */
#include "room_acoustics.h"

int parse_param_file(char *pfile, Pt *source, Wall *wall)
{
    char line[LINE_LEN], *keyword;
    FILE *fp;

    /* open parameter file */
    if ( (fp = fopen(pfile, "r")) == NULL ) {
        fprintf(stderr, "ERROR: Cannot open parameter file %s\n", pfile);
        return -1;
    }

    /* read and parse parameter file */
    source->x = source->y = -1;
    for (int i=0, j=0; fgets(line, LINE_LEN, fp) != NULL; i++) {
        printf("%s", line);
        if ( (keyword = strtok(line, " \n")) != NULL) {
            if ( strncmp(keyword, "sound_source", 12) == 0) {
                source->x = atof(strtok(NULL, " \n"));
                source->y = atof(strtok(NULL, " \n"));
            }
            else if ( strncmp(keyword, "wall", 4) == 0) {
                wall[j].pt[0].x = atof(strtok(NULL, " \n"));
                wall[j].pt[0].y = atof(strtok(NULL, " \n"));
                wall[j].pt[1].x = atof(strtok(NULL, " \n"));
                wall[j].pt[1].y = atof(strtok(NULL, " \n"));
                wall[j].r = atof(strtok(NULL, " \n"));
                j++;
            }
            else {
                fprintf(stderr, "ERROR: unknown parameter keyword %s\n", keyword);
            }
        }
        else {
            fprintf(stderr, "ERROR: empty line in parameter file\n");
        }
    }

    /* print parameters */
    printf("Sound source location (%f, %f)\n", source->x, source->y);
    for (int i=0; i<NUM_WALL; i++) {
        printf("Wall %d: (%f, %f) to (%f, %f) with r of %f\n",
            i,
            wall[i].pt[0].x,
            wall[i].pt[0].y,
            wall[i].pt[1].x,
            wall[i].pt[1].y,
            wall[i].r
            );
    }

    // Close parameter file, with error checking (EOF defined in stdio.h)
    if ( (fclose(fp)) == EOF ) {
        fprintf(stderr, "ERROR: Cannot close parameter file %s\n", pfile);
        return -1;
    }

    return 0;
}
