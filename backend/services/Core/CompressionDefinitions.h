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

        virtual ~CompressedObject() {
            this->m_bytes.clear();
        }
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

        virtual ~IntermediateObject() {
            this->m_locationIndex.clear();
            this->m_byteMap.clear();
        }
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
        virtual void decompress(CompressedObject*& p_sourceObject, SourceObject*& p_decompressedObject) = 0;
    }; //class GeneralCompressionObject

    class GraphCompression : public GeneralCompression {
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            p_sourceObject->getSource()->preprocessing();
            std::unordered_map<uint64_t, uint8_t> visited;
            std::queue<uint64_t> q;

            q.push(0);

            uint8_t inputsBytes, locationIndexByteSize = 0;
            uint64_t currentModule, moduleIndex = 0, inputsSize, inputsTemp;
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
                            inputsTemp = inputsSize;
                            while(inputsTemp != 0) {
                                moduleBytes.emplace_back(static_cast<uint8_t>(inputsTemp));
                                inputsTemp >>= 8;
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
                                inputsTemp = (*this->m_intermediateObject->getLocationIndexPointer())[input.first];
                                for(uint8_t i = 0; i < locationIndexByteSize; i++) {
                                    moduleBytes.emplace_back(static_cast<uint8_t>(inputsTemp));
                                    inputsTemp >>= 8;
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

        virtual void decompress(CompressedObject*& p_sourceObject, SourceObject*& p_decompressedObject) {
            
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
            int64_t rk = 0, rkminus = 0;
            uint64_t d, counter = 0;
            int64_t tempReshape, nk;
            float* ptr;
            float* endPtr;

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
                        counter = 0;
                        frobNorm = 0;
                        while(endPtr != ptr && frobNorm < truncationParam) {
                            el = *--endPtr;
                            frobNorm += el*el;
                            counter++;
                        }
                        rk = (endPtr == ptr)?1:std::min(S.sizes()[0], (int64_t)(S.numel() - counter + 1));

                        U = U.slice(1, 0, rk);
                        S = S.slice(0, 0, rk);
                        V = V.slice(0, 0, rk);
                        
                        S = S.diag();

                        newCore = U.reshape({(int64_t)rkminus, nk, (int64_t)rk});
                        tempTensor = S.matmul(V);

                        cores.emplace_back(newCore.squeeze());

                        rkminus = rk;
                    }
                    cores.emplace_back(tempTensor.squeeze());
                    /*

                        [core size][core data]
                        []
                    */
                    
                }
            }
        }

        virtual void decompress(CompressedObject*& p_sourceObject, SourceObject*& p_decompressedObject) {
            
        }
    }; //class TensorCompression

    class ValueCompression : public GeneralCompression {
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            
        }

        virtual void decompress(CompressedObject*& p_sourceObject, SourceObject*& p_decompressedObject) {
            
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

        std::vector<GeneralCompression*> getCompressionAlgorithms() {
            return this->m_compressionAlgorithms;
        }
        
    }; //class Compressor

    class Decompressor {
    private:
        std::vector<GeneralCompression*> m_compressionAlgorithms;
        CompressedObject* m_sourceObject;
        SourceObject* m_decompressedObject;
    public:
        Decompressor() {}
        Decompressor(std::vector<GeneralCompression*> p_compressionAlgorithms) {
            this->m_compressionAlgorithms = p_compressionAlgorithms;
        }
        Decompressor(std::vector<GeneralCompression*> p_compressionAlgorithms, CompressedObject* p_sourceObject) {
            this->m_compressionAlgorithms = p_compressionAlgorithms;
            this->m_sourceObject = p_sourceObject;
        }

        void decompress() {
            if(this->m_decompressedObject)
                delete this->m_decompressedObject;
            this->m_decompressedObject = new SourceObject();
            for(auto& alg : this->m_compressionAlgorithms) {
                alg->decompress(this->m_sourceObject, this->m_decompressedObject);
            }
        }


    }; //class Decompressor

} //namespace CompressionDefintions
} //namespace Pagoda