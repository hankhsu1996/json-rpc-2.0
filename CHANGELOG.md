# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-08-16

### Added

- Initial release of JSON-RPC 2.0 C++ implementation.
- Implemented core JSON-RPC functionality:
  - Methods and notifications with named and positional parameters.
  - Abstract transport layer support with stdio, HTTP, and ASIO socket connectors.
  - Support for JSON-RPC batch mode.
- Client and server implementation with example files.
- Bazel build system and Conan package management support.
- Doxygen documentation with enhanced branding.
- Unit tests for key components: client, server, request, response, and transport layers.
- ASIO socket and Unix domain socket transport implementations.
- Support for async method calls and non-blocking request handling.
- Google C++ Style alignment and modern C++ practices.
