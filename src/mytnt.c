#include <stdlib.h>
#include <string.h>

#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>

#include "mytnt.h"

MYTNT *mytnt_init(MYTNT *mytnt_arg) {
	MYTNT *mytnt = (MYTNT *) malloc(sizeof(MYTNT));

	mytnt->tnt = NULL;
	mytnt->error_no = 0;
	mytnt->error_info = NULL;

	mytnt->mempool = (msgpack_zone *) malloc(sizeof(msgpack_zone));

	msgpack_zone_init(mytnt->mempool, 2048);

	return mytnt;
}

int mytnt_real_connect(MYTNT *mytnt, const char *host, unsigned int port) {
	if(mytnt->tnt) return mytnt->tnt;
	mytnt->tnt = tnt_net(NULL);

	if(!mytnt->tnt) return -2;

	char uri[128] = {0};
	snprintf(uri, 128, "%s:%d", host, port);
	printf("\t[uri]: %s\n", uri);

	tnt_set(mytnt->tnt, TNT_OPT_URI, "localhost:3301");
	tnt_set(mytnt->tnt, TNT_OPT_SEND_BUF, 0);
	tnt_set(mytnt->tnt, TNT_OPT_RECV_BUF, 0);

	return tnt_connect(mytnt->tnt);
}

MYTNT_STMT *mytnt_stmt_init(MYTNT *mytnt) {
	MYTNT_STMT *mytnt_stmt = (MYTNT_STMT *) malloc(sizeof(MYTNT_STMT));

	mytnt_stmt->mytnt = mytnt;

	mytnt_stmt->query = NULL;
	mytnt_stmt->bind = NULL;
	mytnt_stmt->params = NULL;
	mytnt_stmt->result = NULL;

	mytnt_stmt->it = 0;

	return mytnt_stmt;
}

int mytnt_stmt_prepare(MYTNT_STMT *stmt, const char *query,
		       unsigned long length)
{
	stmt->query = query;
	stmt->query_len = length;
	return 0;
}

void mytnt_close(MYTNT *mytnt) {
	if (mytnt->tnt != NULL) {
		tnt_close(mytnt->tnt);
		tnt_stream_free(mytnt->tnt);
	}

	msgpack_zone_destroy(mytnt->mempool);
	free(mytnt->error_info);
	free(mytnt);
}

int mytnt_stmt_close(MYTNT_STMT * stmt) {
	//если бы stmt->params было в куче, то надо free
	free(stmt);
	return 0;
}

const char *mytnt_error(MYTNT *mytnt) {
	return mytnt->error_info ? mytnt->error_info : "";
}

int mytnt_errno(MYTNT *mytnt) {
	return mytnt->error_no;
}

int mytnt_stmt_bind_param(MYTNT_STMT *stmt, MYTNT_BIND *bnd) {
	stmt->params = bnd;

	stmt->count_params = 0;
	for (int i = 0; stmt->query[i] != '\0'; i++) {
		if (stmt->query[i] == '?')
			stmt->count_params++;
	}

	return 0;
}

int mytnt_stmt_execute(MYTNT_STMT *stmt) {
	struct tnt_stream *params;
	params = tnt_object(NULL);

	if (tnt_object_type(params, TNT_SBO_PACKED) == -1)
		return -1;
	if (tnt_object_add_array(params, 0) == -1)
		return -1;

	for (int i = 0; i < stmt->count_params; ++i) {
		switch (stmt->params[i].buffer_type) {
			case MYTNT_TYPE_FLOAT:
				if (tnt_object_add_float(params, *((float *) stmt->params[i].buffer)) == -1)
					return -1;
				break;
			case MYTNT_TYPE_LONG:
				if (tnt_object_add_int(params, *((int *) stmt->params[i].buffer)) == -1)
					return -1;
				break;
			case MYTNT_TYPE_STRING:
				if (tnt_object_add_strz(params, stmt->params[i].buffer) == -1)
					return -1;
				break;
			default:
				return -2;

		}
	}

	if (tnt_object_container_close(params) == -1)
		return -1;

	tnt_execute(stmt->mytnt->tnt, stmt->query, stmt->query_len, params);
	tnt_flush(stmt->mytnt->tnt);

	tnt_stream_free(params);

	struct tnt_reply *reply = tnt_reply_init(NULL);
	stmt->mytnt->tnt->read_reply(stmt->mytnt->tnt, reply);

	stmt->mytnt->error_no = reply->code;

	if (stmt->mytnt->error_info) {
		free(stmt->mytnt->error_info);
	}

	if (reply->error) {
		stmt->mytnt->error_info = (char *) malloc(1024 * sizeof(char));
		strcpy(stmt->mytnt->error_info, reply->error);
	}

	stmt->result = (msgpack_object *) malloc(sizeof(msgpack_object));

	msgpack_unpack(reply->data, reply->data_end - reply->data, NULL,
		       stmt->mytnt->mempool, stmt->result);

	tnt_reply_free(reply);

	return stmt->mytnt->error_no;
}

int mytnt_stmt_free_result(MYTNT_STMT *stmt) {
	//если бы stmt->bind было в куче, то надо free

	if (stmt->result) {
		free(stmt->result);
		stmt->result = NULL;
	}
	stmt->it = 0;

	return 0;
}

int mytnt_stmt_bind_result(MYTNT_STMT * stmt, MYTNT_BIND * bnd, int count_bind) {
	stmt->bind = bnd;
	stmt->count_bind = count_bind;

	return 0;
}

int mytnt_stmt_fetch(MYTNT_STMT *stmt) {
	if (stmt->result->via.array.size <= stmt->it) {
		return MYTNT_NO_DATA;
	}

	int size;
	char *ptr;
	char *str;

	for (int i = 0; i < stmt->count_bind; ++i) {
		switch (stmt->bind[i].buffer_type) {
		case MYTNT_TYPE_FLOAT:
			*((float *) stmt->bind[i].buffer) = (float) stmt->result->via.array.ptr[stmt->it].via.array.ptr[i].via.f64;
			break;
		case MYTNT_TYPE_LONG:
			*((int *) stmt->bind[i].buffer) = stmt->result->via.array.ptr[stmt->it].via.array.ptr[i].via.i64;
			break;
		case MYTNT_TYPE_STRING:
			size = stmt->result->via.array.ptr[stmt->it].via.array.ptr[i].via.str.size;
			ptr = stmt->result->via.array.ptr[stmt->it].via.array.ptr[i].via.str.ptr;
			str = stmt->bind[i].buffer;

			strncpy(str, ptr, size);
			str[size] = '\0';
			break;
		default:
			return MYTNT_ERROR;

		}
	}

	++(stmt->it);

	return 0;
}
