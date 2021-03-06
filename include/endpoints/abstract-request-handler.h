#pragma once

#include <WebServer.h>
#include "config.h"

extern const char mimeWOFF[];

namespace dbuddy {
    class AbstractRequestHandler : public RequestHandler {
    public:
        explicit AbstractRequestHandler(Config * config) : config(config) {};

        bool canHandle(HTTPMethod method, String uri) override;
        bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) final;
    protected:
        Config * config;

        virtual bool setup(WebServer &server, HTTPMethod requestMethod, String requestUri) = 0;
    };
}
