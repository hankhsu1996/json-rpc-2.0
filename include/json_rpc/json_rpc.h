#pragma once
#ifndef JSON_RPC_JSON_RPC_H
#define JSON_RPC_JSON_RPC_H

// Core components
#include "json_rpc/core/dispatcher.h"
#include "json_rpc/core/request.h"
#include "json_rpc/core/response.h"
#include "json_rpc/core/transport.h"
#include "json_rpc/core/types.h"

// Server component
#include "json_rpc/server/server.h"

// Client component
#include "json_rpc/client/client.h"

// Transport components
#include "json_rpc/transports/http_transport.h"
#include "json_rpc/transports/stdio_transport.h"

#endif // JSON_RPC_JSON_RPC_H
