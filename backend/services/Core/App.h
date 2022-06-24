#pragma once

#include "./crow/crow.h"

#include "GeneralDefinitions.h"
#include "WebDefinitions.h"
#include "CompressionDefinitions.h"
#include "DataDefinitions.h"

namespace Pagoda {

    class App {
    private:
        crow::App<Pagoda::WebDefinitions::MiddleWare> m_app;
        Pagoda::GeneralDefinitions::GeneralNet* m_generalNet;
        Pagoda::DataDefinitions::DataInterface* m_dataInterface;
        Pagoda::CompressionDefintions::Compressor* m_compressor;
    public:
        App() {}

        App(Pagoda::GeneralDefinitions::GeneralNet* p_generalNet, Pagoda::DataDefinitions::DataInterface* p_dataInterface, Pagoda::CompressionDefintions::Compressor* p_compressor) {
            this->m_generalNet = p_generalNet;
            this->m_dataInterface = p_dataInterface;
            this->m_compressor = p_compressor;
        }

        void setup() {
            CROW_ROUTE(this->m_app, "/addModule")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/editModule")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/removeModule")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/addEdge")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });
            
            CROW_ROUTE(this->m_app, "/removeEdge")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/linkDataset")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/startTraining")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });
            
            CROW_ROUTE(this->m_app, "/stopTraining")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/pack")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/unpack")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                return crow::response{""};
            });
        }

        void run() {
            this->m_app.multithreaded().run();
        }
        

        ~App() {}
    }; //class App

} //namespace Pagoda