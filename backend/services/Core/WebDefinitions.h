#pragma once

#include <string>
#include <vector>

#include "./crow/crow.h"

namespace Pagoda {
namespace WebDefinitions {

    struct MiddleWare {
        struct context
        {
        };

        MiddleWare() {}

        void before_handle(crow::request& req, crow::response& res, context& ctx) {

        }

        void after_handle(crow::request& req, crow::response& res, context& ctx) {

        }

    }; //struct MiddleWare

} //namespace WebDefinitions
} //namespace Pagoda