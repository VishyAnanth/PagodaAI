#pragma once

#include <vector>

#include "GeneralDefinitions.h"

namespace Pagoda {
namespace CompressionDefintions {

    class CompressedObject {
    private:
        std::vector<uint8_t> m_bytes;
    public:
        CompressedObject() {}
        
        std::vector<uint8_t> getbytes() {
            return this->m_bytes;
        }
        void setBytes(std::vector<uint8_t> p_bytes) {
            this->m_bytes = p_bytes;
        }

        virtual ~CompressedObject() {}
    }; //class CompressedObject

    class IntermediateObject {
    private:
        std::unordered_map<uint64_t, uint64_t> m_locationIndex;
        std::unordered_map<uint64_t, std::vector<uint8_t> > m_byteMap;
    public:
        IntermediateObject() {}

        std::unordered_map<uint64_t, std::vector<uint8_t> >* getByteMapPointer() {
            return &this->m_byteMap;
        }
        void setByteMap(std::unordered_map<uint64_t, std::vector<uint8_t> > p_byteMap) {
            this->m_byteMap = p_byteMap;
        }

        std::unordered_map<uint64_t, uint64_t>* getLocationIndexPointer() {
            return &this->m_locationIndex;
        }
        void setLocationIndex(std::unordered_map<uint64_t, uint64_t> p_locationIndex) {
            this->m_locationIndex = p_locationIndex;
        }

        virtual ~IntermediateObject() {}
    }; //class IntermediateObject

    class SourceObject {
    private:
        Pagoda::GeneralDefinitions::GeneralNet* m_source;
    public:
        SourceObject() {}

        Pagoda::GeneralDefinitions::GeneralNet* getSource() {
            return this->m_source;
        }
        void setSource(Pagoda::GeneralDefinitions::GeneralNet*& p_source) {
            this->m_source = p_source;
        }

        virtual ~SourceObject() {}
    }; //class SourceObject

    class GeneralCompression {    
    protected:
        IntermediateObject* m_intermediateObject;
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) = 0;
        virtual void decompress(CompressedObject*& p_compressedObject, SourceObject*& p_sourceObject) = 0;
    }; //class GeneralCompressionObject

    class GraphCompression : public GeneralCompression {
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            p_sourceObject->getSource()->preprocessing();
            std::unordered_map<uint64_t, uint8_t> visited;
            std::queue<uint64_t> q;

            q.push(0);

            uint8_t inputsBytes, locationIndexByteSize = 0;
            uint64_t currentModule, moduleIndex = 0, inputsSize;
            std::vector<uint8_t> moduleBytes;

            torch::Tensor t = torch::tensor(1);
    
            while(!q.empty()) {
                currentModule = q.front();
                q.pop();
                
                for(auto& adj : (*p_sourceObject->getSource()->getAdjacencyPointer())[currentModule]) {
                    if(visited[adj] == 0) {
                        p_sourceObject->getSource()->getModules()[adj]->addToInput(currentModule, t);
                        if(p_sourceObject->getSource()->getModules()[adj]->checkInputs()) {
                            q.push(adj);
                            
                            moduleBytes.emplace_back(static_cast<uint8_t>(adj));
                            moduleBytes.emplace_back(static_cast<uint8_t>(adj >> 8));
                            moduleBytes.emplace_back(static_cast<uint8_t>(adj >> 16));
                            moduleBytes.emplace_back(static_cast<uint8_t>(adj >> 24));

                            inputsSize = p_sourceObject->getSource()->getModules()[adj]->getInputsPointer()->size();
                            inputsBytes =   (inputsSize <= 0xFF)?1:
                                            (inputsSize <= 0xFFFF)?2:
                                            (inputsSize <= 0xFFFFFF)?3:
                                            (inputsSize <= 0xFFFFFFFF)?4:
                                            (inputsSize <= 0xFFFFFFFFFF)?5:
                                            (inputsSize <= 0xFFFFFFFFFFFF)?6:
                                            (inputsSize <= 0xFFFFFFFFFFFFFF)?7:8;
                            moduleBytes.emplace_back(inputsBytes);
                            while(inputsSize != 0) {
                                moduleBytes.emplace_back(static_cast<uint8_t>(inputsSize));
                                inputsSize >>= 8;
                            }

                            (*this->m_intermediateObject->getLocationIndexPointer())[adj] = moduleIndex;
                            locationIndexByteSize = std::max(locationIndexByteSize, 
                            (uint8_t)((moduleIndex <= 0xFF)?1:
                                            (moduleIndex <= 0xFFFF)?2:
                                            (moduleIndex <= 0xFFFFFF)?3:
                                            (moduleIndex <= 0xFFFFFFFF)?4:
                                            (moduleIndex <= 0xFFFFFFFFFF)?5:
                                            (moduleIndex <= 0xFFFFFFFFFFFF)?6:
                                            (moduleIndex <= 0xFFFFFFFFFFFFFF)?7:8));
                            moduleIndex++;
                                
                            moduleBytes.emplace_back(locationIndexByteSize);
                            
                            for(auto& input : *p_sourceObject->getSource()->getModules()[adj]->getInputsPointer()) {
                                inputsSize = (*this->m_intermediateObject->getLocationIndexPointer())[input.first];
                                for(uint8_t i = 0; i < locationIndexByteSize; i++) {
                                    moduleBytes.emplace_back(static_cast<uint8_t>(inputsSize));
                                    inputsSize >>= 8;
                                }
                            }

                            (*this->m_intermediateObject->getByteMapPointer())[adj] = moduleBytes;
                            moduleBytes.clear();
                            visited[adj] = 1;
                        }
                    }
                }
            }
        }

        virtual void decompress(CompressedObject*& p_compressedObject, SourceObject*& p_sourceObject) {
            
        }
    }; //class GraphCompression
    
    class TensorCompression : public GeneralCompression {
    private:
        double m_error = 1e-7; //This needs to be determined/learned eventually, setting arbitrarily for now
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            std::vector<uint8_t> moduleBytes;
            std::unordered_map<uint64_t, std::shared_ptr<Pagoda::GeneralDefinitions::GeneralObject> > modules = p_sourceObject->getSource()->getModules();
            std::vector<uint64_t> sizeVec;
            torch::Tensor tempTensor, newCore, U, S, V;
            std::vector<torch::Tensor> cores;
            double frobNorm = 0, truncationParam = 0, el = 0;
            int64_t rk = 0, rkminus = 0, tempReshape, nk;
            uint64_t d, counter = 0, coreSize;
            uint32_t tempCoreElem;
            float* ptr;
            float* endPtr;
            uint8_t coreSizeByte;

            std::unordered_map<uint64_t, std::vector<uint8_t> > byteMapPtr = *this->m_intermediateObject->getByteMapPointer();

            for(auto& elem : byteMapPtr) {
                for(auto& tensor : modules[elem.first]->parameters()) {
                    sizeVec.clear();
                    sizeVec.insert(sizeVec.begin(), tensor.sizes().begin(), tensor.sizes().end());
                    d = sizeVec.size();

                    ptr = tensor.data_ptr<float>();
                    endPtr = ptr + tensor.numel();
                    while(ptr != endPtr) {
                        el = *ptr++;
                        frobNorm += el*el;
                    }
                    truncationParam = this->m_error * frobNorm / (d - 1);
                    truncationParam = (truncationParam == 0)?1:truncationParam;

                    tempTensor = tensor;
                    rkminus = 1;
                    for(uint64_t k = 1; k < d; k++) {
                        nk = sizeVec[k - 1];
                        tempReshape = (int64_t)rkminus * nk;
                        tempTensor = tempTensor.reshape({tempReshape, tempTensor.numel() / tempReshape});
                        auto svd = torch::linalg::svd(tempTensor, false);
                        U = std::get<0>(svd), S = std::get<1>(svd), V = std::get<2>(svd);

                        ptr = S.data_ptr<float>();
                        endPtr = ptr + S.numel();
                        counter = (int64_t)(endPtr - ptr);
                        frobNorm = 0;
                        while(endPtr != ptr && frobNorm < truncationParam) {
                            el = *--endPtr;
                            frobNorm += el*el;
                        }
                        rk = (endPtr == ptr)?1:std::min(S.sizes()[0], (int64_t)(S.numel() - counter + 1));

                        U = U.slice(1, 0, rk);
                        S = S.slice(0, 0, rk);
                        V = V.slice(0, 0, rk);
                        
                        S = S.diag();

                        newCore = U.reshape({(int64_t)rkminus, nk, (int64_t)rk});
                        tempTensor = S.matmul(V);
                        newCore = newCore.squeeze();
                        
                        for(auto it = newCore.sizes().begin(); it != newCore.sizes().end(); it++) {
                            coreSize = *it;
                            coreSizeByte = (uint8_t)((coreSize <= 0xFF)?1:
                                    (coreSize <= 0xFFFF)?2:
                                    (coreSize <= 0xFFFFFF)?3:
                                    (coreSize <= 0xFFFFFFFF)?4:
                                    (coreSize <= 0xFFFFFFFFFF)?5:
                                    (coreSize <= 0xFFFFFFFFFFFF)?6:
                                    (coreSize <= 0xFFFFFFFFFFFFFF)?7:8);
                            moduleBytes.emplace_back(coreSizeByte);
                            while(coreSize != 0) {
                                moduleBytes.emplace_back(static_cast<uint8_t>(coreSize));
                                coreSize >>= 8;
                            }
                        }
                        ptr = newCore.data_ptr<float>();
                        endPtr = newCore.data_ptr<float>() + newCore.numel();
                        while(ptr != endPtr) {
                            tempCoreElem = *(uint32_t*)&(*ptr);
                            moduleBytes.emplace_back(tempCoreElem);
                            moduleBytes.emplace_back(tempCoreElem >> 8);
                            moduleBytes.emplace_back(tempCoreElem >> 16);
                            moduleBytes.emplace_back(tempCoreElem >> 24);
                            ptr++;
                        }
                        rkminus = rk;
                    }
                    newCore = tempTensor.squeeze();
                    for(auto it = newCore.sizes().begin(); it != newCore.sizes().end(); it++) {
                        coreSize = *it;
                        coreSizeByte = (uint8_t)((coreSize <= 0xFF)?1:
                                (coreSize <= 0xFFFF)?2:
                                (coreSize <= 0xFFFFFF)?3:
                                (coreSize <= 0xFFFFFFFF)?4:
                                (coreSize <= 0xFFFFFFFFFF)?5:
                                (coreSize <= 0xFFFFFFFFFFFF)?6:
                                (coreSize <= 0xFFFFFFFFFFFFFF)?7:8);
                        moduleBytes.emplace_back(coreSizeByte);
                        while(coreSize != 0) {
                            moduleBytes.emplace_back(static_cast<uint8_t>(coreSize));
                            coreSize >>= 8;
                        }
                    }
                    ptr = newCore.data_ptr<float>();
                    endPtr = newCore.data_ptr<float>() + newCore.numel();
                    while(ptr != endPtr) {
                        tempCoreElem = *(uint32_t*)&(*ptr);
                        moduleBytes.emplace_back(tempCoreElem);
                        moduleBytes.emplace_back(tempCoreElem >> 8);
                        moduleBytes.emplace_back(tempCoreElem >> 16);
                        moduleBytes.emplace_back(tempCoreElem >> 24);
                        ptr++;
                    }
                }
            }
        }

        virtual void decompress(CompressedObject*& p_compressedObject, SourceObject*& p_sourceObject) {
            
        }
    }; //class TensorCompression

    class ValueCompression : public GeneralCompression {
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            std::unordered_map<uint64_t, std::vector<uint8_t> > byteMapPtr = *this->m_intermediateObject->getByteMapPointer();
            
            for(auto& elem : byteMapPtr) {
                
            }
        }

        virtual void decompress(CompressedObject*& p_compressedObject, SourceObject*& p_sourceObject) {
            
        }
    }; //class ValueCompression

    class Compressor {
    private:
        std::vector<GeneralCompression*> m_compressionAlgorithms;
        SourceObject* m_sourceObject;
        CompressedObject* m_compressedObject;
    public:
        Compressor() {}
        Compressor(std::vector<GeneralCompression*> p_compressionAlgorithms) {
            this->m_compressionAlgorithms = p_compressionAlgorithms;
        }
        Compressor(std::vector<GeneralCompression*> p_compressionAlgorithms, SourceObject* p_sourceObject) {
            this->m_compressionAlgorithms = p_compressionAlgorithms;
            this->m_sourceObject = p_sourceObject;
        }
        
        void compress() {
            if(this->m_compressedObject)
                delete this->m_compressedObject;
            this->m_compressedObject = new CompressedObject();
            for(auto& alg : this->m_compressionAlgorithms) {
                alg->compress(this->m_sourceObject, this->m_compressedObject);
            }
        }

        void decompress() {
            if(this->m_sourceObject)
                delete this->m_sourceObject;
            this->m_sourceObject = new SourceObject();
            uint64_t n = this->m_compressionAlgorithms.size();
            for(uint64_t i = n - 1; i >= 0; i--) {
                this->m_compressionAlgorithms[i]->decompress(this->m_compressedObject, this->m_sourceObject);
            }
        }

        std::vector<GeneralCompression*> getCompressionAlgorithms() {
            return this->m_compressionAlgorithms;
        }
        
    }; //class Compressor

} //namespace CompressionDefintions
} //namespace Pagoda