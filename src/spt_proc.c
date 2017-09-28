/*
 * spt_proc.pc
 * support routines for the proc tpcc implementation
 */

#include "mytnt.h"

#include <stdio.h>

/*
 * report error
 */
int error(
    MYTNT        *mytnt,
    MYTNT_STMT   *mytnt_stmt
)
{
/*
	if(mysql_stmt) {
	    printf("\n%d, %s, %s", mysql_stmt_errno(mysql_stmt),
		   mysql_stmt_sqlstate(mysql_stmt), mysql_stmt_error(mysql_stmt) );
	}
*/
	if(mytnt){
	    fprintf(stderr, "%d, %s\n", mytnt_errno(mytnt), mytnt_error(mytnt) );
	}
	return (0);
}


