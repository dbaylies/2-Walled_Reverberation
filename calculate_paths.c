#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>     /* strtok */
#include <math.h>       /* sqrt */
#include "room_acoustics.h"

int get_delay(Pt *p, int fs)
{
    int samp_delay;
    double len;
    /* listener is at (0,0) */
    len = sqrt( (p->x * p->x) + (p->y * p->y) ) ;
    samp_delay = round(fs * len/SpeedOfSound);
    return samp_delay;
}

int calculate_paths(Pt *source, Wall *wall, Path *path, Pt *image, int fs)
{
    /* calculate direct path and images */
    /* direct path */
    path[direct_p].delay = get_delay(source, fs);
    path[direct_p].atten = 1.0;

    /* calculate images */
    /* reflection from front wall */
    image[front_i].x = source->x;
    image[front_i].y = wall[front_w].pt[front_w].y +
        (wall[front_w].pt[front_w].y - source->y);
    if (fs > 0) {
        path[front_p].delay = get_delay(&image[front_i], fs);
        path[front_p].atten = 1.0 * wall[front_w].r;
    }

    /* reflection from side wall */
    image[side_i].x = wall[side_w].pt[0].x + (wall[side_w].pt[0].x - source->x);
    image[side_i].y = source->y;
    if (fs > 0) {
        path[side_p].delay = get_delay(&image[side_i], fs);
        path[side_p].atten = 1.0 * wall[side_w].r;
    }

    /* reflection from two walls */
    image[both_i].x = wall[side_w].pt[0].x + (wall[side_w].pt[0].x - source->x);
    image[both_i].y = wall[front_w].pt[0].y + (wall[front_w].pt[0].y - source->y);
    if (fs > 0) {
        path[both_p].delay = get_delay(&image[both_i], fs);
        path[both_p].atten = 1.0 * wall[front_w].r * wall[side_w].r;
    }

    if (fs > 0) {
        /* find maximum image delay */
        int max_delay = 0;
        for (int i=0; i<NUM_PATH; i++) {
            printf("Path %d: delay %d, atten %f\n", i, 
                path[i].delay, path[i].atten);
            if (path[i].delay > max_delay) {
                max_delay = path[i].delay;
            }
        }
        printf("Maximum delay of reflections is %d samples\n", max_delay);
        return max_delay;
    }
    else {
        return 0;
    }
}
