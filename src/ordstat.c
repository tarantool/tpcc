/*
 * -*-C-*- 
 * ordstat.pc 
 * corresponds to A.3 in appendix A
 */

#include <string.h>
#include <stdio.h>

#include "mytnt.h"

#include "spt_proc.h"
#include "tpc.h"

extern MYTNT **ctx;
extern MYTNT_STMT ***stmt;

/*
 * the order status transaction
 */
int ordstat( int t_num,
	     int w_id_arg,		/* warehouse id */
	     int d_id_arg,		/* district id */
	     int byname,		/* select by c_id or c_last? */
	     int c_id_arg,		/* customer id */
	     char c_last_arg[]	        /* customer last name, format? */
)
{
	int            w_id = w_id_arg;
	int            d_id = d_id_arg;
	int            c_id = c_id_arg;
	int            c_d_id = d_id;
	int            c_w_id = w_id;
	char            c_first[17];
	char            c_middle[3];
	char            c_last[17];
	float           c_balance;
	int            o_id;
	char            o_entry_d[25];
	int            o_carrier_id;
	int            ol_i_id;
	int            ol_supply_w_id;
	int            ol_quantity;
	float           ol_amount;
	char            ol_delivery_d[25]; // здесь пустой (до выполнения [29])
	int            namecnt;

	int             n;
	int             proceed = 0;

	MYTNT_STMT*   mytnt_stmt;
        MYTNT_BIND    param[6];
        MYTNT_BIND    column[5];

	/*EXEC SQL WHENEVER NOT FOUND GOTO sqlerr;*/
	/*EXEC SQL WHENEVER SQLERROR GOTO sqlerr;*/

	if (byname) {
		strcpy(c_last, c_last_arg);
		proceed = 1;
		/*EXEC_SQL SELECT count(c_id)
			INTO :namecnt
		        FROM customer
			WHERE c_w_id = :c_w_id
			AND c_d_id = :c_d_id
		        AND c_last = :c_last;*/
		mytnt_stmt = stmt[t_num][20];

                memset(param, 0, sizeof(MYTNT_BIND) * 3); /* initialize */
                param[0].buffer_type = MYTNT_TYPE_LONG;
                param[0].buffer = &c_w_id;
                param[1].buffer_type = MYTNT_TYPE_LONG;
                param[1].buffer = &c_d_id;
                param[2].buffer_type = MYTNT_TYPE_STRING;
                param[2].buffer = c_last;
                param[2].buffer_length = strlen(c_last);
                if( mytnt_stmt_bind_param(mytnt_stmt, param) ) goto sqlerr;
                if( mytnt_stmt_execute(mytnt_stmt) ) goto sqlerr;

//                if( mysql_stmt_store_result(mysql_stmt) ) goto sqlerr;
                memset(column, 0, sizeof(MYTNT_BIND) * 1); /* initialize */
                column[0].buffer_type = MYTNT_TYPE_LONG;
                column[0].buffer = &namecnt;
                if( mytnt_stmt_bind_result(mytnt_stmt, column, 1) ) goto sqlerr;
                switch( mytnt_stmt_fetch(mytnt_stmt) ) {
                    case 0: //SUCCESS
                        break;
                    case 1: //ERROR
                    case MYTNT_NO_DATA: //NO MORE DATA
                    default:
                        mytnt_stmt_free_result(mytnt_stmt);
                        goto sqlerr;
                }
                mytnt_stmt_free_result(mytnt_stmt);


		proceed = 2;
		/*EXEC_SQL DECLARE c_byname_o CURSOR FOR
		        SELECT c_balance, c_first, c_middle, c_last
		        FROM customer
		        WHERE c_w_id = :c_w_id
			AND c_d_id = :c_d_id
			AND c_last = :c_last
			ORDER BY c_first;
		proceed = 3;
		EXEC_SQL OPEN c_byname_o;*/
		mytnt_stmt = stmt[t_num][21];

		memset(param, 0, sizeof(MYTNT_BIND) * 3); /* initialize */
		param[0].buffer_type = MYTNT_TYPE_LONG;
                param[0].buffer = &c_w_id;
                param[1].buffer_type = MYTNT_TYPE_LONG;
                param[1].buffer = &c_d_id;
                param[2].buffer_type = MYTNT_TYPE_STRING;
                param[2].buffer = c_last;
                param[2].buffer_length = strlen(c_last);
                if( mytnt_stmt_bind_param(mytnt_stmt, param) ) goto sqlerr;
                if( mytnt_stmt_execute(mytnt_stmt) ) goto sqlerr;

//		if( mysql_stmt_store_result(mysql_stmt) ) goto sqlerr;
                memset(column, 0, sizeof(MYTNT_BIND) * 4); /* initialize */
		column[0].buffer_type = MYTNT_TYPE_FLOAT;
		column[0].buffer = &c_balance;
		column[1].buffer_type = MYTNT_TYPE_STRING;
		column[1].buffer = c_first;
		column[1].buffer_length = sizeof(c_first);
		column[2].buffer_type = MYTNT_TYPE_STRING;
		column[2].buffer = c_middle;
		column[2].buffer_length = sizeof(c_middle);
		column[3].buffer_type = MYTNT_TYPE_STRING;
		column[3].buffer = c_last;
		column[3].buffer_length = sizeof(c_last);
		if( mytnt_stmt_bind_result(mytnt_stmt, column, 4) ) goto sqlerr;

		if (namecnt % 2)
			namecnt++;	/* Locate midpoint customer; */

		for (n = 0; n < namecnt / 2; n++) {
			proceed = 4;
			/*EXEC_SQL FETCH c_byname_o
			  INTO :c_balance, :c_first, :c_middle, :c_last;*/
			switch( mytnt_stmt_fetch(mytnt_stmt) ) {
			    case 0: //SUCCESS
//                            case MYSQL_DATA_TRUNCATED:
				break;
			    case 1: //ERROR
			    case MYTNT_NO_DATA: //NO MORE DATA
			    default:
				mytnt_stmt_free_result(mytnt_stmt);
				goto sqlerr;
			}
		}
		proceed = 5;
		/*EXEC_SQL CLOSE  c_byname_o;*/
		mytnt_stmt_free_result(mytnt_stmt);


	} else {		/* by number */
		proceed = 6;
		/*EXEC_SQL SELECT c_balance, c_first, c_middle, c_last
			INTO :c_balance, :c_first, :c_middle, :c_last
		        FROM customer
		        WHERE c_w_id = :c_w_id
			AND c_d_id = :c_d_id
			AND c_id = :c_id;*/
		mytnt_stmt = stmt[t_num][22];

		memset(param, 0, sizeof(MYTNT_BIND) * 3); /* initialize */
		param[0].buffer_type = MYTNT_TYPE_LONG;
		param[0].buffer = &c_w_id;
		param[1].buffer_type = MYTNT_TYPE_LONG;
		param[1].buffer = &c_d_id;
		param[2].buffer_type = MYTNT_TYPE_LONG;
		param[2].buffer = &c_id;
		if( mytnt_stmt_bind_param(mytnt_stmt, param) ) goto sqlerr;
		if( mytnt_stmt_execute(mytnt_stmt) ) goto sqlerr;

//		if( mysql_stmt_store_result(mysql_stmt) ) goto sqlerr;
		memset(column, 0, sizeof(MYTNT_BIND) * 4); /* initialize */
		column[0].buffer_type = MYTNT_TYPE_FLOAT;
		column[0].buffer = &c_balance;
		column[1].buffer_type = MYTNT_TYPE_STRING;
		column[1].buffer = c_first;
		column[1].buffer_length = sizeof(c_first);
		column[2].buffer_type = MYTNT_TYPE_STRING;
		column[2].buffer = c_middle;
		column[2].buffer_length = sizeof(c_middle);
		column[3].buffer_type = MYTNT_TYPE_STRING;
		column[3].buffer = c_last;
		column[3].buffer_length = sizeof(c_last);
		if( mytnt_stmt_bind_result(mytnt_stmt, column, 4) ) goto sqlerr;
		switch( mytnt_stmt_fetch(mytnt_stmt) ) {
		    case 0: //SUCCESS
//                    case MYSQL_DATA_TRUNCATED:
			break;
		    case 1: //ERROR
		    case MYTNT_NO_DATA: //NO MORE DATA
		    default:
			mytnt_stmt_free_result(mytnt_stmt);
			goto sqlerr;
		}
		mytnt_stmt_free_result(mytnt_stmt);

	}

	/* find the most recent order for this customer */

	proceed = 7;
	/*EXEC_SQL SELECT o_id, o_entry_d, COALESCE(o_carrier_id,0)
		INTO :o_id, :o_entry_d, :o_carrier_id
	        FROM orders
	        WHERE o_w_id = :c_w_id
		AND o_d_id = :c_d_id
		AND o_c_id = :c_id
		AND o_id = (SELECT MAX(o_id)
		    	    FROM orders
		    	    WHERE o_w_id = :c_w_id
		  	    AND o_d_id = :c_d_id
		    	    AND o_c_id = :c_id);*/
	mytnt_stmt = stmt[t_num][23];

	memset(param, 0, sizeof(MYTNT_BIND) * 6); /* initialize */
	param[0].buffer_type = MYTNT_TYPE_LONG;
	param[0].buffer = &c_w_id;
	param[1].buffer_type = MYTNT_TYPE_LONG;
	param[1].buffer = &c_d_id;
	param[2].buffer_type = MYTNT_TYPE_LONG;
	param[2].buffer = &c_id;
	param[3].buffer_type = MYTNT_TYPE_LONG;
	param[3].buffer = &c_w_id;
	param[4].buffer_type = MYTNT_TYPE_LONG;
	param[4].buffer = &c_d_id;
	param[5].buffer_type = MYTNT_TYPE_LONG;
	param[5].buffer = &c_id;
	if( mytnt_stmt_bind_param(mytnt_stmt, param) ) goto sqlerr;
	if( mytnt_stmt_execute(mytnt_stmt) ) goto sqlerr;

//	if( mysql_stmt_store_result(mysql_stmt) ) goto sqlerr;
	memset(column, 0, sizeof(MYTNT_BIND) * 3); /* initialize */
	column[0].buffer_type = MYTNT_TYPE_LONG;
        column[0].buffer = &o_id;
	column[1].buffer_type = MYTNT_TYPE_STRING;
	column[1].buffer = o_entry_d;
	column[1].buffer_length = sizeof(o_entry_d);
	column[2].buffer_type = MYTNT_TYPE_LONG;
        column[2].buffer = &o_carrier_id;
	if( mytnt_stmt_bind_result(mytnt_stmt, column, 3) ) goto sqlerr;
        switch( mytnt_stmt_fetch(mytnt_stmt) ) {
            case 0: //SUCCESS
                break;
            case 1: //ERROR
            case MYTNT_NO_DATA: //NO MORE DATA
            default:
                mytnt_stmt_free_result(mytnt_stmt);
                goto sqlerr;
        }
        mytnt_stmt_free_result(mytnt_stmt);


	/* find all the items in this order */
	proceed = 8;
	/*EXEC_SQL DECLARE c_items CURSOR FOR
		SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount,
                       ol_delivery_d
		FROM order_line
	        WHERE ol_w_id = :c_w_id
		AND ol_d_id = :c_d_id
		AND ol_o_id = :o_id;*/
	mytnt_stmt = stmt[t_num][24];

	memset(param, 0, sizeof(MYTNT_BIND) * 3); /* initialize */
	param[0].buffer_type = MYTNT_TYPE_LONG;
	param[0].buffer = &c_w_id;
	param[1].buffer_type = MYTNT_TYPE_LONG;
	param[1].buffer = &c_d_id;
	param[2].buffer_type = MYTNT_TYPE_LONG;
	param[2].buffer = &o_id;
	if( mytnt_stmt_bind_param(mytnt_stmt, param) ) goto sqlerr;
	if( mytnt_stmt_execute(mytnt_stmt) ) goto sqlerr;

//	if( mysql_stmt_store_result(mysql_stmt) ) goto sqlerr;
        memset(column, 0, sizeof(MYTNT_BIND) * 5); /* initialize */
	column[0].buffer_type = MYTNT_TYPE_LONG;
        column[0].buffer = &ol_i_id;
	column[1].buffer_type = MYTNT_TYPE_LONG;
        column[1].buffer = &ol_supply_w_id;
	column[2].buffer_type = MYTNT_TYPE_LONG;
        column[2].buffer = &ol_quantity;
	column[3].buffer_type = MYTNT_TYPE_FLOAT;
        column[3].buffer = &ol_amount;
//	column[4].buffer_type = MYTNT_TYPE_STRING;
//        column[4].buffer = ol_delivery_d;
//        column[4].buffer_length = sizeof(ol_delivery_d);
	if( mytnt_stmt_bind_result(mytnt_stmt, column, 4) ) goto sqlerr;

	/*proceed = 9;
	EXEC_SQL OPEN c_items;

	EXEC SQL WHENEVER NOT FOUND GOTO done;*/
	for (;;) {
		proceed = 10;
		/*EXEC_SQL FETCH c_items
			INTO :ol_i_id, :ol_supply_w_id, :ol_quantity,
			:ol_amount, :ol_delivery_d;*/
		switch( mytnt_stmt_fetch(mytnt_stmt) ) {
		    case 0: //SUCCESS
//		    case MYSQL_DATA_TRUNCATED:
			break;
		    case MYTNT_NO_DATA: //NO MORE DATA
			mytnt_stmt_free_result(mytnt_stmt);
			goto done;
		    case 1: //ERROR
		    default:
			mytnt_stmt_free_result(mytnt_stmt);
			goto sqlerr;
		}
	}

done:
	/*EXEC_SQL CLOSE c_items;*/
        /*EXEC_SQL COMMIT WORK;*/
//	if( mysql_commit(ctx[t_num]) ) goto sqlerr;

	return (1);

sqlerr:
        fprintf(stderr, "ordstat %d:%d\n",t_num,proceed);
	error(ctx[t_num], mytnt_stmt);
        /*EXEC SQL WHENEVER SQLERROR GOTO sqlerrerr;*/
	/*EXEC_SQL ROLLBACK WORK;*/
//	mysql_rollback(ctx[t_num]);
sqlerrerr:
	return (0);
}

