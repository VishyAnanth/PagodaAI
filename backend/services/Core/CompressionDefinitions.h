#pragma once

#include <vector>

#include "GeneralDefinitions.h"

namespace Pagoda {
namespace CompressionDefintions {

    class CompressedObject {
    private:
        std::vector<uint8_t> m_bytes;
        std::vector<float> m_values;
    public:
        CompressedObject() {}
        
        std::vector<uint8_t> getbytes() {
            return this->m_bytes;
        }
        void setBytes(std::vector<uint8_t> p_bytes) {
            this->m_bytes = p_bytes;
        }

        std::vector<float> getValues() {
            return this->m_values;
        }
        void setValues(std::vector<float> p_values) {
            this->m_values = p_values;
        }
        
        virtual ~CompressedObject() {}
    }; //class CompressedObject

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
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) = 0;
        virtual void decompress(CompressedObject*& p_sourceObject, SourceObject*& p_decompressedObject) = 0;
    }; //class GeneralCompressionObject

    class GraphCompression : public GeneralCompression {
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            Pagoda::GeneralDefinitions::GeneralNet* net = p_sourceObject->getSource();
            
        }

        virtual void decompress(CompressedObject*& p_sourceObject, SourceObject*& p_decompressedObject) {

        }
    }; //class GraphCompression
    
    class TensorCompression : public GeneralCompression {
    public:
        virtual void compress(SourceObject*& p_sourceObject, CompressedObject*& p_compressedObject) {
            Pagoda::GeneralDefinitions::GeneralNet* net = p_sourceObject->getSource();
            std::vector<float> tenRepresentation;
            for(auto& module : net->getModules()) {
                for(auto& ten : module.second->parameters()) {
                    std::copy(ten.data_ptr<float>(), ten.data_ptr<float>() + ten.numel(), tenRepresentation.begin());
                    
                    

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