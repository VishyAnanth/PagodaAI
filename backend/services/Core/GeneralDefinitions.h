#pragma once

#include <torch/torch.h>
#include <unordered_map>
#include <set>
#include <queue>
#include <stack>
#include <iostream>
#include <cstdarg>
#include <math.h>

namespace Pagoda {
namespace GeneralDefinitions {

    class GeneralObject : public torch::nn::Module {
    protected:
        uint64_t m_id;
        std::vector<uint64_t> m_moduleParams;
        std::unordered_map<uint64_t, torch::Tensor> m_inputs;
        std::set<uint64_t> m_expectedInputs;
    public:
        uint64_t getId() {
            return this->m_id;
        }

        void setId(uint64_t p_id) {
            this->m_id = p_id;
        }

        void setModuleParams(uint64_t p_argc, ... ) {
            va_list args;
            va_start(args, p_argc);

            this->m_moduleParams.clear();
            uint64_t arg;

            for(uint8_t i = 0; i < p_argc; i++) {
                arg = va_arg(args, uint64_t);
                this->m_moduleParams.emplace_back(arg);
            }
        }

        void addToInput(uint64_t p_source, torch::Tensor& p_input) {
            this->m_expectedInputs.erase(p_source);
            this->m_inputs[p_source] = p_input;
        }

        void removeInput(uint64_t p_source) {
            this->m_inputs.erase(p_source);
        }

        void clearInputs() {
            this->m_expectedInputs.clear();
            this->m_inputs.clear();
        }

        uint8_t checkInputs() {
            return this->m_expectedInputs.size() == 0;
        }

        std::unordered_map<uint64_t, torch::Tensor>* getInputsPointer() {
            return &this->m_inputs;
        }

        void addExpectedInput(uint64_t p_source) {
            this->m_expectedInputs.insert(p_source);
        }

        void removeExpectedInput(uint64_t p_source) {
            this->m_expectedInputs.erase(p_source);
        }

        virtual torch::Tensor forward() = 0;
        virtual ~GeneralObject() = default;
    }; //class GeneralObject

    class GeneralNet : public GeneralObject {
    private:
        std::unordered_map<uint64_t, std::shared_ptr<GeneralObject> > m_modules;
        std::unordered_map<uint64_t, std::set<uint64_t> > m_adjacency;
        std::unordered_map<uint64_t, std::set<uint64_t> > m_adjacencyTranspose;
        uint64_t m_idValue;
    public:
        GeneralNet() {
            this->m_idValue = 1;
        }

        std::unordered_map<uint64_t, std::set<uint64_t> >* getAdjacencyPointer() {
            return &this->m_adjacency;
        }
        std::unordered_map<uint64_t, std::set<uint64_t> >* getAdjacencyTransposePointer() {
            return &this->m_adjacencyTranspose;
        }

        std::unordered_map<uint64_t, std::shared_ptr<GeneralObject> > getModules() {
            return this->m_modules;
        }

        void insertModule(std::shared_ptr<GeneralObject> p_module) {
            p_module->setId((p_module->getId()) + ((++this->m_idValue)<<32));
            this->m_modules[p_module->getId()] = register_module(std::to_string(p_module->getId()), p_module);
        }

        uint8_t addEdge(uint64_t p_source, uint64_t p_destination) {
            if(this->m_adjacency[p_source].find(p_destination) != this->m_adjacency[p_source].end()
            || this->m_adjacency[p_destination].find(p_source) != this->m_adjacency[p_destination].end()
            || p_source == p_destination)
                return 0;
            this->m_adjacency[p_source].insert(p_destination);
            this->m_adjacencyTranspose[p_destination].insert(p_source);
            if(this->cycleDetection()) {
                this->removeEdge(p_source, p_destination);
                return 0;
            }
            return 1;
        }

        void removeEdge(uint64_t p_source, uint64_t p_destination) {
            this->m_adjacency[p_source].erase(p_destination);
            this->m_adjacencyTranspose[p_destination].erase(p_source);
        }

        void removeModule(uint64_t p_module) {
            for(auto& itr : this->m_adjacency[p_module]) {
                this->m_adjacencyTranspose[itr].erase(p_module);
            }
            this->m_adjacency.erase(p_module);

            for(auto& itr : this->m_adjacencyTranspose[p_module]) {
                this->m_adjacency[itr].erase(p_module);
            }
            this->m_adjacencyTranspose.erase(p_module);
        }

        uint8_t cycleDetection() {
            std::unordered_map<uint64_t, uint64_t> inDegree;
            for(auto& mod : this->m_modules) {
                for(auto& adj : this->m_adjacency[mod.first]) {
                    inDegree[adj]++;
                }
            }

            std::queue<uint64_t> q;
            for(auto& mod : this->m_modules) {
                if(inDegree[mod.first] == 0) 
                    q.push(mod.first);
            }

            uint64_t counter = 1;
            uint64_t currentModule;
            while(!q.empty()) {
                currentModule = q.front();
                q.pop();

                for(auto& itr : this->m_adjacency[currentModule]) {
                    if(--inDegree[itr] == 0) {
                        q.push(itr);
                        counter++;
                    }
                }
            }
            return counter != this->m_modules.size();
        }

        void addToInput(uint64_t p_source, torch::Tensor& p_input) {
            this->m_expectedInputs.erase(p_source);
            this->m_inputs[0] = p_input;
        }

        void preprocessing() {
            for(auto& itr : this->m_adjacencyTranspose) {
                this->m_modules[itr.first]->clearInputs();
                for(auto& input : itr.second) {
                    this->m_modules[itr.first]->addExpectedInput(input);
                }
            }
        }

        void postprocessing() {
            for(auto& itr : this->m_modules) {
                itr.second->clearInputs();
            }
        }

        torch::Tensor forward() {
            this->preprocessing();
            std::unordered_map<uint64_t, uint8_t> visited;
            std::queue<uint64_t> q;
            std::queue<torch::Tensor> qTensor;

            q.push(0);
            qTensor.push(this->m_inputs[0]);

            uint64_t currentModule;
            torch::Tensor currentTensor;
            while(!q.empty()) {
                currentModule = q.front();
                currentTensor = qTensor.front();

                q.pop();
                qTensor.pop();

                for(auto& adj : this->m_adjacency[currentModule]) {
                    if(visited[adj] == 0) {
                        this->m_modules[adj]->addToInput(currentModule, currentTensor);
                        if(this->m_modules[adj]->checkInputs()) {
                            q.push(adj);
                            qTensor.push(this->m_modules[adj]->forward());
                            visited[adj] = 1;
                        }
                    }
                }
            }
            this->postprocessing();
            return currentTensor;
        }

    }; //class GeneralNet

} //namespace GeneralDefintions
} //namespace Pagoda