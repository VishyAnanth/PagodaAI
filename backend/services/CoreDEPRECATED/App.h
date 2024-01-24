#pragma once

#include <string>

#include "./crow/crow.h"

#include "GeneralDefinitions.h"
#include "WebDefinitions.h"
#include "CompressionDefinitions.h"
#include "DataDefinitions.h"
#include "StorageDefinitions.h"
#include "ModuleDefinitions.h"

namespace Pagoda {

    class App {
    private:
        crow::App<Pagoda::WebDefinitions::MiddleWare> m_app;
        Pagoda::GeneralDefinitions::GeneralNet* m_generalNet;
        Pagoda::DataDefinitions::DataInterface* m_dataInterface;
        Pagoda::StorageDefinitions::StorageInterface* m_storageInterface;
        Pagoda::CompressionDefintions::Compressor* m_compressor;
    public:
        App() {}

        App(Pagoda::GeneralDefinitions::GeneralNet* p_generalNet, Pagoda::DataDefinitions::DataInterface* p_dataInterface, Pagoda::StorageDefinitions::StorageInterface* p_storageInterface, Pagoda::CompressionDefintions::Compressor* p_compressor) {
            this->m_generalNet = p_generalNet;
            this->m_dataInterface = p_dataInterface;
            this->m_storageInterface = p_storageInterface;
            this->m_compressor = p_compressor;
        }

        std::shared_ptr<Pagoda::GeneralDefinitions::GeneralObject> objectCreator(crow::json::rvalue& jsonBody) {
            uint32_t id = jsonBody["id"].i();
            std::shared_ptr<Pagoda::GeneralDefinitions::GeneralObject> newObj;
            switch(id) {
                case 1:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::FullyConnectedModule>(jsonBody["n"].i(),jsonBody["m"].i(),jsonBody["o"].i(),jsonBody["bias"].i(),jsonBody["type"].i());
                    break;
                case 2:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::ConvolutionModule>(jsonBody["in"].i(), jsonBody["out"].i(), jsonBody["kernel"].i(), jsonBody["stride"].i(), jsonBody["bias"].i(), jsonBody["type"].i());
                    break;
                case 3:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::EmbeddingModule>(jsonBody["num"].i(), jsonBody["size"].i(), jsonBody["padding"].i(), jsonBody["maxNorm"].i(), jsonBody["normType"].i());
                    break;
                case 4:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::RecurrentModule>(jsonBody["input"].i(), jsonBody["hidden"].i(), jsonBody["layers"].i(), jsonBody["dropout"].d(), jsonBody["bidirectional"].i(), jsonBody["type"].i());
                    break;
                case 5:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::TransformerModule>(jsonBody["inputs"].i(), jsonBody["heads"].i(), jsonBody["encoderLayers"].i(), jsonBody["decoderLayers"].i());
                    break;
                case 6:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::PoolingModule>(jsonBody["x"].i(), jsonBody["y"].i(), jsonBody["z"].i(), jsonBody["strideX"].i(), jsonBody["strideY"].i(), jsonBody["strideZ"].i(), jsonBody["norm"].i(), jsonBody["type"].i());
                    break;
                case 7:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::PaddingModule>(jsonBody["p1"].i(), jsonBody["p2"].i(), jsonBody["p3"].i(), jsonBody["p4"].i(), jsonBody["p5"].i(), jsonBody["p6"].i(), jsonBody["p7"].d(), jsonBody["type"].i());
                    break;
                case 8:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::NormalizationModule>(jsonBody["numFeatures1"].i(), jsonBody["numFeatures2"].i(), jsonBody["epsilon"].d(), jsonBody["momentum"].d(), jsonBody["alpha"].d(), jsonBody["beta"].d(), jsonBody["type"].i());
                    break;
                case 9:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::DropoutModule>(jsonBody["p"].d(), jsonBody["type"].i());
                    break;
                case 10:
                    newObj = std::make_shared<Pagoda::ModuleDefinitions::ActivationModule>(jsonBody["param1"].d(), jsonBody["param2"].d(), jsonBody["param3"].i(), jsonBody["param4"].i(), jsonBody["param5"].d(), jsonBody["bias"].i(), jsonBody["type"].i());
                    break;
                default:
                    newObj = this->m_storageInterface->getObject(id);
                    break;
            }
            return newObj;
        }

        void setup() {
            CROW_ROUTE(this->m_app, "/addModule")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);
                
                this->m_generalNet->insertModule(this->objectCreator(jsonBody));
                
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/editModule")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);
                uint64_t id = jsonBody["id"].i();
                if(this->m_generalNet->checkIdExists(id)) {
                    this->m_generalNet->editModule(id, this->objectCreator(jsonBody));
                }
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/removeModule")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);
                uint64_t id = jsonBody["id"].i();
                if(this->m_generalNet->checkIdExists(id)) {
                    this->m_generalNet->removeModule(id);
                }
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/addEdge")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);
                uint64_t source = jsonBody["source"].i();
                uint64_t dest = jsonBody["dest"].i();
                if(this->m_generalNet->checkIdExists(source) && this->m_generalNet->checkIdExists(dest)) {
                    if(!this->m_generalNet->addEdge(source, dest)) {
                        return crow::response{"ERROR"};
                    }
                }
                return crow::response{""};
            });
            
            CROW_ROUTE(this->m_app, "/removeEdge")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);
                uint64_t source = jsonBody["source"].i();
                uint64_t dest = jsonBody["dest"].i();
                if(this->m_generalNet->checkIdExists(source) && this->m_generalNet->checkIdExists(dest)) {
                    this->m_generalNet->removeEdge(source, dest);
                }
                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/linkDataset")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);

                

                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/startTraining")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);


                
                return crow::response{""};
            });
            
            CROW_ROUTE(this->m_app, "/stopTraining")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);



                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/pack")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);



                return crow::response{""};
            });

            CROW_ROUTE(this->m_app, "/unpack")
            .methods("POST"_method)
            ([this](const crow::request& req) {
                auto jsonBody = crow::json::load(req.body);
                if (!jsonBody)
                    return crow::response(400);

                

                return crow::response{""};
            });
        }

        void run() {
            this->m_app.multithreaded().run();
        }
        

        ~App() {}
    }; //class App

} //namespace Pagoda