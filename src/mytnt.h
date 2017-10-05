#ifndef TPCC_TARANTOOL_MYTNT_H
#define TPCC_TARANTOOL_MYTNT_H

#include <msgpack.h>

#define MYTNT_NO_DATA	100
#define MYTNT_ERROR	1

typedef enum mytnt_enum_field_types {
	MYTNT_TYPE_DECIMAL, MYTNT_TYPE_TINY,
	MYTNT_TYPE_SHORT,  MYTNT_TYPE_LONG,
	MYTNT_TYPE_FLOAT,  MYTNT_TYPE_DOUBLE,
	MYTNT_TYPE_NULL,   MYTNT_TYPE_TIMESTAMP,
	MYTNT_TYPE_LONGLONG,MYTNT_TYPE_INT24,
	MYTNT_TYPE_DATE,   MYTNT_TYPE_TIME,
	MYTNT_TYPE_DATETIME, MYTNT_TYPE_YEAR,
	MYTNT_TYPE_NEWDATE, MYTNT_TYPE_VARCHAR,
	MYTNT_TYPE_BIT,
	MYTNT_TYPE_TIMESTAMP2,
	MYTNT_TYPE_DATETIME2,
	MYTNT_TYPE_TIME2,
	MYTNT_TYPE_JSON=245,
	MYTNT_TYPE_NEWDECIMAL=246,
	MYTNT_TYPE_ENUM=247,
	MYTNT_TYPE_SET=248,
	MYTNT_TYPE_TINY_BLOB=249,
	MYTNT_TYPE_MEDIUM_BLOB=250,
	MYTNT_TYPE_LONG_BLOB=251,
	MYTNT_TYPE_BLOB=252,
	MYTNT_TYPE_VAR_STRING=253,
	MYTNT_TYPE_STRING=254,
	MYTNT_TYPE_GEOMETRY=255
} mytnt_enum_field_types;

typedef struct {
	struct tnt_stream *tnt;
	char 		*error_info;
	int 		error_no;
	msgpack_zone 	*mempool;
} MYTNT;

typedef struct {
	enum	mytnt_enum_field_types buffer_type;
	void	*buffer;
	size_t	buffer_length;
} MYTNT_BIND;

typedef struct {
	int		it;		/* iterator by results */
	char 		*query;
	int		query_len;
	int		count_params;
	int		count_bind;
	MYTNT		*mytnt;
	MYTNT_BIND	*params;	/* input parameters */
	MYTNT_BIND	*bind;		/* output parameters */
	msgpack_object	*result;
} MYTNT_STMT;

int mytnt_stmt_execute(MYTNT_STMT *stmt);

const char *mytnt_error(MYTNT *mytnt);

int mytnt_errno(MYTNT *mytnt);

MYTNT *mytnt_init(MYTNT *mytnt);

int mytnt_real_connect(MYTNT *mytnt, const char *host, unsigned int port);

MYTNT_STMT *mytnt_stmt_init(MYTNT *mytnt);

int mytnt_stmt_prepare(MYTNT_STMT *stmt, const char *query,
		       unsigned long length);

void mytnt_close(MYTNT *sock);

int mytnt_stmt_close(MYTNT_STMT * stmt);

int mytnt_stmt_bind_param(MYTNT_STMT *stmt, MYTNT_BIND *bnd);

int mytnt_stmt_free_result(MYTNT_STMT *stmt);

int mytnt_stmt_bind_result(MYTNT_STMT * stmt, MYTNT_BIND * bnd, int count_bind);

int mytnt_stmt_fetch(MYTNT_STMT *stmt);

#endif //TPCC_TARANTOOL_MYTNT_H
