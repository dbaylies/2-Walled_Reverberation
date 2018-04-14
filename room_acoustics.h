#define LINE_LEN        80      /* of parameter file */
#define SpeedOfSound    343.0   /* meters/second, speed of sound in air */
#define BLOCK_LEN       1024    /* processing block length */

/* index on walls */
enum {
    front_w = 0,
    side_w,
    NUM_WALL
};

/* index on images */
enum {
    front_i = 0,
    side_i,
    both_i,
    NUM_IMAGE
};

/* index on paths */
enum {
    direct_p = 0,
    front_p,
    side_p,
    both_p,
    NUM_PATH
};

/* index for two-wall reflection paths */
#define both_f both_i
#define both_s (both_i+1)

typedef struct {
    double x;
    double y;
} Pt;

typedef struct {
    Pt pt[2]; //two points, one at each end of wall segment
    double r; //reflectivity of wall
} Wall;

typedef struct {
    unsigned int delay; //delay of image source, in samples
    double atten; //attenuation of image source
} Path;

int parse_param_file(char *pfile, Pt *source, Wall *wall);
int calculate_paths(Pt *source, Wall *wall, Path *path, Pt *image, int fs);
