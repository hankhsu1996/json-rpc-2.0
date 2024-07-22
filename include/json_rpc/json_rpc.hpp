#pragma once

// Core components
#include "json_rpc/core/client_transport.hpp"
#include "json_rpc/core/dispatcher.hpp"
#include "json_rpc/core/request.hpp"
#include "json_rpc/core/response.hpp"
#include "json_rpc/core/server_transport.hpp"
#include "json_rpc/core/types.hpp"

// Server component
#include "json_rpc/server/server.hpp"

// Client component
#include "json_rpc/client/client.hpp"

// Transport components
#include "json_rpc/transports/framed_stdio_client_transport.hpp"
#include "json_rpc/transports/framed_stdio_server_transport.hpp"
#include "json_rpc/transports/framed_transport.hpp"
#include "json_rpc/transports/http_client_transport.hpp"
#include "json_rpc/transports/http_server_transport.hpp"
#include "json_rpc/transports/stdio_client_transport.hpp"
#include "json_rpc/transports/stdio_server_transport.hpp"
