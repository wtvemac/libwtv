#include "libc/printf.h"

struct __sFile
{
	int unused;
};

typedef struct __sFILE FILE;


/* Standard streams.  */
//extern FILE *stdin;  /* Standard input stream.  */
//extern FILE *stdout; /* Standard output stream.  */
//extern FILE *stderr; /* Standard error output stream.  */

/* C89/C99 say they're macros.  Make them happy.  */
//#define stdin stdin
//#define stdout stdout
//#define stderr stderr
#define stdin 1
#define stdout 2
#define stderr 3
