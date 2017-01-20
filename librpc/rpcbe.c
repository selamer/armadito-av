/***

Copyright (C) 2015, 2016 Teclib'

This file is part of Armadito core.

Armadito core is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Armadito core is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Armadito core.  If not, see <http://www.gnu.org/licenses/>.

***/

#include <libjrpc/jrpc.h>

#include "core/info.h"
#include "core/ondemand.h"
#include "rpc/rpctypes.h"

#include <glib.h>

static gpointer scan_thread_fun(gpointer data)
{
	struct a6o_on_demand *on_demand = (struct a6o_on_demand *)data;

	a6o_on_demand_run(on_demand);

	a6o_on_demand_free(on_demand);

	return NULL;
}

static int scan_method(json_t *params, json_t **result, void *connection_data)
{
	struct armadito *armadito = (struct armadito *)connection_data;
	int ret;
	struct a6o_rpc_scan_param *s_param;
	struct a6o_on_demand *on_demand;

	if ((ret = JRPC_JSON2STRUCT(a6o_rpc_scan_param, params, &s_param)))
		return ret;

	a6o_log(A6O_LOG_SERVICE, A6O_LOG_LEVEL_DEBUG, "scan path %s", s_param->root_path);

	/* FIXME */
	/* must get period from params */
	on_demand = a6o_on_demand_new(armadito, s_param->root_path, A6O_SCAN_RECURSE | A6O_SCAN_THREADED, 0);

	g_thread_new("scan thread", scan_thread_fun, on_demand);

	return JRPC_OK;
}

static int status_method(json_t *params, json_t **result, void *connection_data)
{
	struct armadito *armadito = (struct armadito *)connection_data;
	int ret;
	struct a6o_info *info;

	info = a6o_info_new(armadito);

	if ((ret = JRPC_STRUCT2JSON(a6o_info, info, result)))
		return ret;

	a6o_info_free(info);

	return JRPC_OK;
}

static struct jrpc_mapper *rpcbe_mapper;

static void create_rpcbe_mapper(void)
{
	rpcbe_mapper = jrpc_mapper_new();
	jrpc_mapper_add(rpcbe_mapper, "scan", scan_method);
	jrpc_mapper_add(rpcbe_mapper, "status", status_method);

	/* jrpc_mapper_add_error_message(rpcbe_mapper, ERR_DIVIDE_BY_ZERO, "divide by zero"); */
	/* jrpc_mapper_add_error_message(rpcbe_mapper, ERR_SQRT_OF_NEGATIVE, "square root of negative number"); */
	/* jrpc_mapper_add_error_message(rpcbe_mapper, ERR_INVALID_ACTION, "invalid notification action"); */
}

struct jrpc_mapper *a6o_get_rpcbe_mapper(void)
{
	if (rpcbe_mapper == NULL)
		create_rpcbe_mapper();

	return rpcbe_mapper;
}
