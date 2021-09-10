/* minimal usage of libpq */

#include <libpq-fe.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	(void) printf("PQlibVersion=%d\n", PQlibVersion());
	return (0);
}
