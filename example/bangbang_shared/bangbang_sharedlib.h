/* Header file to be used by callers of this functionality */

#define NUM_REF 5
#define HMAC_DIGEST_SIZE 32

/* Function Prototypes for Line_Follower.c */
char * read_raw();
int uobj_read_raw(char *buffer);
int * read_analog(int trys);
int * read_digital();
float * get_average(int mount);
int * found_line_in(float timeout);
void wait_tile_status(int *status);
void wait_tile_center();
int lib_init(void);
int lib_exit(void);
