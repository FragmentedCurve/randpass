/*
	A utility for generating random passwords (and ASCII strings). Unicode
	is not support.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>

// nstrings.h is not a completed library yet, but good enough for us.
#define NSTRINGS_MAIN
#include "nstrings.h"

#define POOL_MAX 1024        // Hard limit on how many pools for the sample space.

#define RAND_BUF_MAX   1024
#define RAND_TYPE_DEV  1     // Use /dev/random
#define RAND_TYPE_LIBC 2     // Use srand() & rand()

// Structure for collecting strings to be used as a sample space.
typedef struct {
	size_t length;         // Total length of all pools (sample size).
	size_t count;          // Quantity of sample pools in pool.
	string pool[POOL_MAX]; // Array of character sets to randomly select chars from.
} samplespace;

typedef struct {
	int type;              // Either RAND_TYPE_DEV or RAND_TYPE_LIBC.
	int fd;                // File descriptor for /dev/random.
	size_t i;              // Index of buf.
	int buf[RAND_BUF_MAX]; // Cached random integers.
} random_context;

// Global variables & default values
samplespace    samples         = {0};  // A global data structure 'cause we're a tiny prog.
random_context ctx             = {0};  // Context struct for RNG.
bool           show_newline    = true; // Print a newline character afterward password.
size_t         password_length = 8;    // Length of randomly generated password.

void
samplespace_append(samplespace* samples, string s)
{
	// If the user hit this limit, they don't deserve an error
	// message.
	if (samples->count < POOL_MAX) {
		samples->pool[samples->count] = s;
		samples->length += s.length;
		samples->count++;
	}
}

int
random_int(random_context* ctx)
{
	if (ctx->type == RAND_TYPE_LIBC)
		return rand();

	if (ctx->i == RAND_BUF_MAX) {
		ctx->i = 1;
		read(ctx->fd, ctx->buf, sizeof(ctx->buf));
		return ctx->buf[0];
	}

	return ctx->buf[ctx->i++];	
}

void
random_init(random_context* ctx)
{
	memset(ctx, 0, sizeof(random_context));

	// Attempt using /dev/random first.
	ctx->fd = open("/dev/random", O_RDONLY);
	if (ctx->fd < 0) {
		// Fall back on libc.
		ctx->type = RAND_TYPE_LIBC;
		srand(time(NULL));
		return;
	}

	ctx->type = RAND_TYPE_DEV;
	read(ctx->fd, ctx->buf, sizeof(ctx->buf));
}

// Choose a random byte (no unicode) from the sample space.
char
random_choice(random_context* ctx, samplespace* sample)
{
	unsigned int x = (unsigned int) random_int(ctx) % sample->length;
	string* p = sample->pool;

	while (x >= p->length) {
		x -= p->length;
		++p;
	}

	return p->s[x];
}

// Everyone needs help at some point.
void
usage(string cmd) {
	PrintErr$(
		"%s [options]\n"
		"Options:\n"
		"  -n           Don't print newline\n"
		"  -c           Length of password\n"
		"  -a           Tiny alphabet\n"
		"  -A           Big alphabet\n"
		"  -N           Numeric\n"
		"  -s           Symbols\n"
		"  -r [chars]   Custom character list\n"
		"  -h           Display this\n", cmd);
}

int
Main(int argc, string* argv)
{
	if (argc <= 1) {
		usage(argv[0]);
		exit(-1);
	}

	const string TINYALPHA = $("abcdefghijklmnopqrstuvwxyz");
	const string BIGALPHA  = $("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	const string NUMERIC   = $("0123456789");
	const string SYMBOL    = $("~!@#$%^&*()_+`-=[];',./{}|:\"<>?\\");

	// Parse command arguments
	for (int i = 1; i < argc; ++i) {
		if (Equals$("-c", argv[i])) {
			if (argc <= ++i) {
				PrintErr$("%S", "An argument must be supplied to '-c'.\n");
				exit(-1);
			}
			// This is safe because argv[i] originally was
			// a null-terminated string.
			password_length = atoi(argv[i].s);
		} else if (Equals$("-r", argv[i])) {
			if (argc <= ++i) {
				PrintErr$("%S", "An argument must be supplied to '-r'.\n");
				exit(-1);
			}
			samplespace_append(&samples, argv[i]);
		} else if (HasPrefix$("-", argv[i])) {
			for (int j = 1; j < argv[i].length; ++j) {
				switch (argv[i].s[j]) {
				case 'a':
					samplespace_append(&samples, TINYALPHA);
					break;
				case 'A':
					samplespace_append(&samples, BIGALPHA);
					break;
				case 'N':
					samplespace_append(&samples, NUMERIC);
					break;
				case 's':
					samplespace_append(&samples, SYMBOL);
					break;
				case 'n':
					show_newline = false;
					break;
				case 'h':
					usage(argv[0]);
					exit(0);
				case 'r': // Fall-through
				case 'c':
					PrintErr$("Flag '-%c' cannot be grouped.\n", argv[i].s[j]);
					exit(-1);
				default:
					PrintErr$("Invalid flag '-%c'.\n", argv[i].s[j]);
					exit(-1);
				}
			}
		} else {
			PrintErr$("Invalid flag '%s'.", argv[i]);
			exit(-1);
		}
	}

	if (samples.count == 0) {
		usage(argv[0]);
		exit(-1);
	}
	
	// Generate random string (or password)
	random_init(&ctx);
	for (size_t i = 0; i < password_length; ++i) {
		// Using stdio.h because nstring.h doesn't buffer.
		putchar(random_choice(&ctx, &samples));
	}

	fflush(stdout);

	if (show_newline) {
		putchar('\n');
	}

	exit(0);
}
