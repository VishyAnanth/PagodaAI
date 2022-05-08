#pragma once

#include "GeneralDefinitions.h"

#include <iostream>

namespace Pagoda {
namespace ModuleDefinitions {

    class FullyConnectedModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::Linear m_linearModule{nullptr};
        torch::nn::Bilinear m_bilinearModule{nullptr};
        /*
                                                                                                                                                            Consecutive 0s      Consecutive 1s
            id = 1      0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1     16                  16                  
            n = 500     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 0 1 0 0     48                  32
            m = 1000    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 0 1 0 0 0     48                  32
            o = 100     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 1 0 0     48                  32
            bias = 0    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0     8                   8                   
            sku = 1     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1     16                  16
                                                                                                                                                            =184                =136
        */

    public:
        FullyConnectedModule(uint64_t n = 1, uint64_t m = 1, uint64_t o = -1, bool bias = true, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_linearModule = register_module(std::to_string(this->m_id), torch::nn::Linear(torch::nn::LinearOptions(n, m).bias(bias)));
                    break;
                case 2:
                    this->m_bilinearModule = register_module(std::to_string(this->m_id), torch::nn::Bilinear(torch::nn::BilinearOptions(n,m,o).bias(bias)));
                    break;
                default:
                    break;
            }
            this->setId(1);
            this->calculateModuleParams(5, n, m, o, bias, type);
        }

        void replaceModule(uint64_t n = 1, uint64_t m = 1, uint64_t o = -1, bool bias = true, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_linearModule = replace_module(std::to_string(this->m_id), torch::nn::Linear(torch::nn::LinearOptions(n, m).bias(bias)));
                    this->m_bilinearModule = torch::nn::Bilinear{nullptr};
                    break;
                case 2:
                    this->m_bilinearModule = replace_module(std::to_string(this->m_id), torch::nn::Bilinear(torch::nn::BilinearOptions(n,m,o).bias(bias)));
                    this->m_linearModule = torch::nn::Linear{nullptr};
                    break;
                default:
                    break;
            }
            this->setId(1);
            this->calculateModuleParams(5, n, m, o, bias, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_linearModule) {
                return this->m_linearModule(itr->second);
            } else {
                return this->m_bilinearModule((++itr)->second, itr->second);
            }
        }
    }; //class FullyConnectedModule

    class ConvolutionModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::Conv1d m_conv1d{nullptr};
        torch::nn::Conv2d m_conv2d{nullptr};
        torch::nn::Conv3d m_conv3d{nullptr};
        torch::nn::ConvTranspose1d m_convTranspose1d{nullptr};
        torch::nn::ConvTranspose2d m_convTranspose2d{nullptr};
        torch::nn::ConvTranspose3d m_convTranspose3d{nullptr};
    public:
        ConvolutionModule(uint64_t in = 1, uint64_t out = 1, uint64_t kernel = 1, uint64_t stride = 1, bool bias = true, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_conv1d = register_module(std::to_string(this->m_id), torch::nn::Conv1d(torch::nn::Conv1dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 2:
                    this->m_conv2d = register_module(std::to_string(this->m_id), torch::nn::Conv2d(torch::nn::Conv2dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 3:
                    this->m_conv3d = register_module(std::to_string(this->m_id), torch::nn::Conv3d(torch::nn::Conv3dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 4:
                    this->m_convTranspose1d = register_module(std::to_string(this->m_id), torch::nn::ConvTranspose1d(torch::nn::ConvTranspose1dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 5:
                    this->m_convTranspose2d = register_module(std::to_string(this->m_id), torch::nn::ConvTranspose2d(torch::nn::ConvTranspose2dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 6:
                    this->m_convTranspose3d = register_module(std::to_string(this->m_id), torch::nn::ConvTranspose3d(torch::nn::ConvTranspose3dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                default:
                    break;
            }
            this->setId(2);
            this->calculateModuleParams(6, in, out, kernel, stride, bias, type);
        }

        void replaceModule(uint64_t in = 1, uint64_t out = 1, uint64_t kernel = 1, uint64_t stride = 1, bool bias = true, uint8_t type = 1) { 
            this->m_conv1d = torch::nn::Conv1d{nullptr};
            this->m_conv2d = torch::nn::Conv2d{nullptr};
            this->m_conv3d = torch::nn::Conv3d{nullptr};
            this->m_convTranspose1d = torch::nn::ConvTranspose1d{nullptr};
            this->m_convTranspose2d = torch::nn::ConvTranspose2d{nullptr};
            this->m_convTranspose3d = torch::nn::ConvTranspose3d{nullptr};
            switch(type) {
                case 1:
                    this->m_conv1d = replace_module(std::to_string(this->m_id), torch::nn::Conv1d(torch::nn::Conv1dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 2:
                    this->m_conv2d = replace_module(std::to_string(this->m_id), torch::nn::Conv2d(torch::nn::Conv2dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 3:
                    this->m_conv3d = replace_module(std::to_string(this->m_id), torch::nn::Conv3d(torch::nn::Conv3dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 4:
                    this->m_convTranspose1d = replace_module(std::to_string(this->m_id), torch::nn::ConvTranspose1d(torch::nn::ConvTranspose1dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 5:
                    this->m_convTranspose2d = replace_module(std::to_string(this->m_id), torch::nn::ConvTranspose2d(torch::nn::ConvTranspose2dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                case 6:
                    this->m_convTranspose3d = replace_module(std::to_string(this->m_id), torch::nn::ConvTranspose3d(torch::nn::ConvTranspose3dOptions(in, out, kernel).stride(stride).bias(bias)));
                    break;
                default:
                    break;
            }
            this->setId(2);
            this->calculateModuleParams(6, in, out, kernel, stride, bias, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_conv1d) {
                return this->m_conv1d(itr->second);
            } else if(this->m_conv2d) {
                return this->m_conv2d(itr->second);
            } else if(this->m_conv3d) {
                return this->m_conv3d(itr->second);
            } else if(this->m_convTranspose1d) {
                return this->m_convTranspose1d(itr->second);
            } else if(this->m_convTranspose2d) {
                return this->m_convTranspose2d(itr->second);
            } else {
                return this->m_convTranspose3d(itr->second);
            }
        }
    }; //class ConvolutionModule

    class EmbeddingModule : public Pagoda::GeneralDefinitions::GeneralObject  {
    private:
        torch::nn::Embedding m_embedding{nullptr};
    public:
        EmbeddingModule(uint64_t num = 1, uint64_t size = 1, uint64_t padding = 0, uint64_t maxNorm = 1, uint64_t normType = 2) {
            this->m_embedding = register_module(std::to_string(this->m_id), torch::nn::Embedding(torch::nn::EmbeddingOptions(num, size).padding_idx(padding).max_norm(maxNorm).norm_type(normType)));
            this->setId(3);
            this->calculateModuleParams(5, num, size, padding, maxNorm, normType);
        }

        void replaceModule(uint64_t num = 1, uint64_t size = 1, uint64_t padding = 0, uint64_t maxNorm = 1, uint64_t normType = 2) {
            this->m_embedding = replace_module(std::to_string(this->m_id), torch::nn::Embedding(torch::nn::EmbeddingOptions(num, size).padding_idx(padding).max_norm(maxNorm).norm_type(normType)));
            this->setId(3);
            this->calculateModuleParams(5, num, size, padding, maxNorm, normType);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            return this->m_embedding(itr->second);
        }
    }; //class EmbeddingModule

    class RecurrentModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::RNN m_rnn{nullptr};
        torch::nn::GRU m_gru{nullptr};
        torch::nn::LSTM m_lstm{nullptr};
    public:
        RecurrentModule(uint64_t input = 1, uint64_t hidden = 1, uint64_t layers = 1, double dropout = 0, bool bidirectional = false, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_rnn = register_module(std::to_string(this->m_id), torch::nn::RNN(torch::nn::RNNOptions(input, hidden).num_layers(layers).dropout(dropout).bidirectional(bidirectional)));
                    break;
                case 2:
                    this->m_gru = register_module(std::to_string(this->m_id), torch::nn::GRU(torch::nn::GRUOptions(input, hidden).num_layers(layers).dropout(dropout).bidirectional(bidirectional)));
                    break;
                case 3:
                    this->m_lstm = register_module(std::to_string(this->m_id), torch::nn::LSTM(torch::nn::LSTMOptions(input, hidden).num_layers(layers).dropout(dropout).bidirectional(bidirectional)));
                    break;
                default:
                    break;
            }
            this->setId(4);
            this->calculateModuleParams(6, input, hidden, layers, *(uint64_t*)&dropout, bidirectional, type);
        }

        void replaceModule(uint64_t input = 1, uint64_t hidden = 1, uint64_t layers = 1, double dropout = 0, bool bidirectional = false, uint8_t type = 1) {
            this->m_rnn = torch::nn::RNN{nullptr};
            this->m_gru = torch::nn::GRU{nullptr};
            this->m_lstm = torch::nn::LSTM{nullptr};
            switch(type) {
                case 1:
                    this->m_rnn = replace_module(std::to_string(this->m_id), torch::nn::RNN(torch::nn::RNNOptions(input, hidden).num_layers(layers).dropout(dropout).bidirectional(bidirectional)));
                    break;
                case 2:
                    this->m_gru = replace_module(std::to_string(this->m_id), torch::nn::GRU(torch::nn::GRUOptions(input, hidden).num_layers(layers).dropout(dropout).bidirectional(bidirectional)));
                    break;
                case 3:
                    this->m_lstm = replace_module(std::to_string(this->m_id), torch::nn::LSTM(torch::nn::LSTMOptions(input, hidden).num_layers(layers).dropout(dropout).bidirectional(bidirectional)));
                    break;
                default:
                    break;
            }
            this->setId(4);
            this->calculateModuleParams(6, input, hidden, layers, *(uint64_t*)&dropout, bidirectional, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_rnn) {
                return std::get<0>(this->m_rnn(itr->second)).index({torch::indexing::Slice(), -1});
            } else if(this->m_gru) {
                return std::get<0>(this->m_gru(itr->second)).index({torch::indexing::Slice(), -1});
            } else {
                return std::get<0>(this->m_lstm(itr->second)).index({torch::indexing::Slice(), -1});
            }
        }
    }; //class RecurrentModule

    class TransformerModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::Transformer m_transformer{nullptr};
    public:
        TransformerModule(uint64_t inputs = 1, uint64_t heads = 1, uint64_t encoderLayers = 1, uint64_t decoderLayers = 1) {
            this->m_transformer = register_module(std::to_string(this->m_id), torch::nn::Transformer(torch::nn::TransformerOptions(inputs, heads).num_encoder_layers(encoderLayers).num_decoder_layers(decoderLayers)));
            this->setId(5);
            this->calculateModuleParams(4, inputs, heads, encoderLayers, decoderLayers);
        }

        void replaceModule(uint64_t inputs = 1, uint64_t heads = 1, uint64_t encoderLayers = 1, uint64_t decoderLayers = 1) {
            this->m_transformer = replace_module(std::to_string(this->m_id), torch::nn::Transformer(torch::nn::TransformerOptions(inputs, heads).num_encoder_layers(encoderLayers).num_decoder_layers(decoderLayers)));
            this->setId(5);
            this->calculateModuleParams(4, inputs, heads, encoderLayers, decoderLayers);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            return this->m_transformer((++itr)->second, itr->second);
        }
    }; //class TransformerModule

    class PoolingModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::MaxPool1d m_maxPool1d{nullptr};
        torch::nn::MaxPool2d m_maxPool2d{nullptr};
        torch::nn::MaxPool3d m_maxPool3d{nullptr};
        torch::nn::AvgPool1d m_avgPool1d{nullptr};
        torch::nn::AvgPool2d m_avgPool2d{nullptr};
        torch::nn::AvgPool3d m_avgPool3d{nullptr};
        torch::nn::FractionalMaxPool2d m_fractionalMaxPool2d{nullptr};
        torch::nn::FractionalMaxPool3d m_fractionalMaxPool3d{nullptr};
        torch::nn::LPPool1d m_lpPool1d{nullptr};
        torch::nn::LPPool2d m_lpPool2d{nullptr};
        torch::nn::AdaptiveMaxPool1d m_adaptiveMaxPool1d{nullptr};
        torch::nn::AdaptiveMaxPool2d m_adaptiveMaxPool2d{nullptr};
        torch::nn::AdaptiveMaxPool3d m_adaptiveMaxPool3d{nullptr};
        torch::nn::AdaptiveAvgPool1d m_adaptiveAvgPool1d{nullptr};
        torch::nn::AdaptiveAvgPool2d m_adaptiveAvgPool2d{nullptr};
        torch::nn::AdaptiveAvgPool3d m_adaptiveAvgPool3d{nullptr};
    public:
        PoolingModule(uint64_t x, uint64_t y, uint64_t z, uint64_t strideX, uint64_t strideY, uint64_t strideZ, uint64_t norm, uint8_t type) {
            switch(type) {
                case 1:
                    this->m_maxPool1d = register_module(std::to_string(this->m_id), torch::nn::MaxPool1d(torch::nn::MaxPool1dOptions(x).stride(strideX)));
                    break;
                case 2:
                    this->m_maxPool2d = register_module(std::to_string(this->m_id), torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY)})));
                    break;
                case 3:
                    this->m_maxPool3d = register_module(std::to_string(this->m_id), torch::nn::MaxPool3d(torch::nn::MaxPool3dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y),static_cast<int64_t>(z)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY),static_cast<int64_t>(strideZ)})));
                    break;
                case 4:
                    this->m_avgPool1d = register_module(std::to_string(this->m_id), torch::nn::AvgPool1d(torch::nn::AvgPool1dOptions(x).stride(strideX)));
                    break;
                case 5:
                    this->m_avgPool2d = register_module(std::to_string(this->m_id), torch::nn::AvgPool2d(torch::nn::AvgPool2dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY)})));
                    break;
                case 6:
                    this->m_avgPool3d = register_module(std::to_string(this->m_id), torch::nn::AvgPool3d(torch::nn::AvgPool3dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y),static_cast<int64_t>(z)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY),static_cast<int64_t>(strideZ)})));
                    break;
                case 7:
                    this->m_fractionalMaxPool2d = register_module(std::to_string(this->m_id), torch::nn::FractionalMaxPool2d(torch::nn::FractionalMaxPool2dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y)})));
                    break;
                case 8:
                    this->m_fractionalMaxPool3d = register_module(std::to_string(this->m_id), torch::nn::FractionalMaxPool3d(torch::nn::FractionalMaxPool3dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y),static_cast<int64_t>(z)})));
                    break;
                case 9:
                    this->m_lpPool1d = register_module(std::to_string(this->m_id), torch::nn::LPPool1d(torch::nn::LPPool1dOptions(norm, x).stride(strideX)));
                    break;
                case 10:
                    this->m_lpPool2d = register_module(std::to_string(this->m_id), torch::nn::LPPool2d(torch::nn::LPPool2dOptions(norm, {static_cast<int64_t>(x), static_cast<int64_t>(y)}).stride({static_cast<int64_t>(strideX), static_cast<int64_t>(strideY)})));
                    break;
                case 11:
                    this->m_adaptiveMaxPool1d = register_module(std::to_string(this->m_id), torch::nn::AdaptiveMaxPool1d(torch::nn::AdaptiveMaxPool1dOptions(x)));
                    break;
                case 12:
                    this->m_adaptiveMaxPool2d = register_module(std::to_string(this->m_id), torch::nn::AdaptiveMaxPool2d(torch::nn::AdaptiveMaxPool2dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y)})));
                    break;
                case 13:
                    this->m_adaptiveMaxPool3d = register_module(std::to_string(this->m_id), torch::nn::AdaptiveMaxPool3d(torch::nn::AdaptiveMaxPool3dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y), static_cast<int64_t>(z)})));
                    break;
                case 14:
                    this->m_adaptiveAvgPool1d = register_module(std::to_string(this->m_id), torch::nn::AdaptiveAvgPool1d(torch::nn::AdaptiveAvgPool1dOptions(x)));
                    break;
                case 15:
                    this->m_adaptiveAvgPool2d = register_module(std::to_string(this->m_id), torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y)})));
                    break;
                case 16:
                    this->m_adaptiveAvgPool3d = register_module(std::to_string(this->m_id), torch::nn::AdaptiveAvgPool3d(torch::nn::AdaptiveAvgPool3dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y), static_cast<int64_t>(z)})));
                    break;
                default:
                    break;
            }
            this->setId(6);
            this->calculateModuleParams(8, x, y, z, strideX, strideY, strideZ, norm, type);
        }

        void replaceModule(uint64_t x, uint64_t y, uint64_t z, uint64_t strideX, uint64_t strideY, uint64_t strideZ, uint64_t norm, uint8_t type) {
            this->m_maxPool1d = torch::nn::MaxPool1d{nullptr};
            this->m_maxPool2d = torch::nn::MaxPool2d{nullptr};
            this->m_maxPool3d = torch::nn::MaxPool3d{nullptr};
            this->m_avgPool1d = torch::nn::AvgPool1d{nullptr};
            this->m_avgPool2d = torch::nn::AvgPool2d{nullptr};
            this->m_avgPool3d = torch::nn::AvgPool3d{nullptr};
            this->m_fractionalMaxPool2d = torch::nn::FractionalMaxPool2d{nullptr};
            this->m_fractionalMaxPool3d = torch::nn::FractionalMaxPool3d{nullptr};
            this->m_lpPool1d = torch::nn::LPPool1d{nullptr};
            this->m_lpPool2d = torch::nn::LPPool2d{nullptr};
            this->m_adaptiveMaxPool1d = torch::nn::AdaptiveMaxPool1d{nullptr};
            this->m_adaptiveMaxPool2d = torch::nn::AdaptiveMaxPool2d{nullptr};
            this->m_adaptiveMaxPool3d = torch::nn::AdaptiveMaxPool3d{nullptr};
            this->m_adaptiveAvgPool1d = torch::nn::AdaptiveAvgPool1d{nullptr};
            this->m_adaptiveAvgPool2d = torch::nn::AdaptiveAvgPool2d{nullptr};
            this->m_adaptiveAvgPool3d = torch::nn::AdaptiveAvgPool3d{nullptr};

            switch(type) {
                case 1:
                    this->m_maxPool1d = replace_module(std::to_string(this->m_id), torch::nn::MaxPool1d(torch::nn::MaxPool1dOptions(x).stride(strideX)));
                    break;
                case 2:
                    this->m_maxPool2d = replace_module(std::to_string(this->m_id), torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY)})));
                    break;
                case 3:
                    this->m_maxPool3d = replace_module(std::to_string(this->m_id), torch::nn::MaxPool3d(torch::nn::MaxPool3dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y),static_cast<int64_t>(z)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY),static_cast<int64_t>(strideZ)})));
                    break;
                case 4:
                    this->m_avgPool1d = replace_module(std::to_string(this->m_id), torch::nn::AvgPool1d(torch::nn::AvgPool1dOptions(x).stride(strideX)));
                    break;
                case 5:
                    this->m_avgPool2d = replace_module(std::to_string(this->m_id), torch::nn::AvgPool2d(torch::nn::AvgPool2dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY)})));
                    break;
                case 6:
                    this->m_avgPool3d = replace_module(std::to_string(this->m_id), torch::nn::AvgPool3d(torch::nn::AvgPool3dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y),static_cast<int64_t>(z)}).stride({static_cast<int64_t>(strideX),static_cast<int64_t>(strideY),static_cast<int64_t>(strideZ)})));
                    break;
                case 7:
                    this->m_fractionalMaxPool2d = replace_module(std::to_string(this->m_id), torch::nn::FractionalMaxPool2d(torch::nn::FractionalMaxPool2dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y)})));
                    break;
                case 8:
                    this->m_fractionalMaxPool3d = replace_module(std::to_string(this->m_id), torch::nn::FractionalMaxPool3d(torch::nn::FractionalMaxPool3dOptions({static_cast<int64_t>(x),static_cast<int64_t>(y),static_cast<int64_t>(z)})));
                    break;
                case 9:
                    this->m_lpPool1d = replace_module(std::to_string(this->m_id), torch::nn::LPPool1d(torch::nn::LPPool1dOptions(norm, x).stride(strideX)));
                    break;
                case 10:
                    this->m_lpPool2d = replace_module(std::to_string(this->m_id), torch::nn::LPPool2d(torch::nn::LPPool2dOptions(norm, {static_cast<int64_t>(x), static_cast<int64_t>(y)}).stride({static_cast<int64_t>(strideX), static_cast<int64_t>(strideY)})));
                    break;
                case 11:
                    this->m_adaptiveMaxPool1d = replace_module(std::to_string(this->m_id), torch::nn::AdaptiveMaxPool1d(torch::nn::AdaptiveMaxPool1dOptions(x)));
                    break;
                case 12:
                    this->m_adaptiveMaxPool2d = replace_module(std::to_string(this->m_id), torch::nn::AdaptiveMaxPool2d(torch::nn::AdaptiveMaxPool2dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y)})));
                    break;
                case 13:
                    this->m_adaptiveMaxPool3d = replace_module(std::to_string(this->m_id), torch::nn::AdaptiveMaxPool3d(torch::nn::AdaptiveMaxPool3dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y), static_cast<int64_t>(z)})));
                    break;
                case 14:
                    this->m_adaptiveAvgPool1d = replace_module(std::to_string(this->m_id), torch::nn::AdaptiveAvgPool1d(torch::nn::AdaptiveAvgPool1dOptions(x)));
                    break;
                case 15:
                    this->m_adaptiveAvgPool2d = replace_module(std::to_string(this->m_id), torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y)})));
                    break;
                case 16:
                    this->m_adaptiveAvgPool3d = replace_module(std::to_string(this->m_id), torch::nn::AdaptiveAvgPool3d(torch::nn::AdaptiveAvgPool3dOptions({static_cast<int64_t>(x), static_cast<int64_t>(y), static_cast<int64_t>(z)})));
                    break;
                default:
                    break;
            }
            this->setId(6);
            this->calculateModuleParams(8, x, y, z, strideX, strideY, strideZ, norm, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_maxPool1d) {
                return this->m_maxPool1d(itr->second);
            } else if(this->m_maxPool2d) {
                return this->m_maxPool2d(itr->second);
            } else if(this->m_maxPool3d) {
                return this->m_maxPool3d(itr->second);
            } else if(this->m_avgPool1d) {
                return this->m_avgPool1d(itr->second);
            } else if(this->m_avgPool2d) {
                return this->m_avgPool2d(itr->second);
            } else if(this->m_avgPool3d) {
                return this->m_avgPool3d(itr->second);
            } else if(this->m_fractionalMaxPool2d) {
                return this->m_fractionalMaxPool2d(itr->second);
            } else if(this->m_fractionalMaxPool3d) {
                return this->m_fractionalMaxPool3d(itr->second);
            } else if(this->m_lpPool1d) {
                return this->m_lpPool1d(itr->second);
            } else if(this->m_lpPool2d) {
                return this->m_lpPool2d(itr->second);
            } else if(this->m_adaptiveMaxPool1d) {
                return this->m_adaptiveMaxPool1d(itr->second);
            } else if(this->m_adaptiveMaxPool2d) {
                return this->m_adaptiveMaxPool2d(itr->second);
            } else if(this->m_adaptiveMaxPool3d) {
                return this->m_adaptiveMaxPool3d(itr->second);
            } else if(this->m_adaptiveAvgPool1d) {
                return this->m_adaptiveAvgPool1d(itr->second);
            } else if(this->m_adaptiveAvgPool2d) {
                return this->m_adaptiveAvgPool2d(itr->second);
            } else {
                return this->m_adaptiveAvgPool3d(itr->second);
            }
        }
    }; //class PoolingModule

    class PaddingModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::ReflectionPad1d m_reflectionPad1d{nullptr};
        torch::nn::ReflectionPad2d m_reflectionPad2d{nullptr};
        torch::nn::ReflectionPad3d m_reflectionPad3d{nullptr};
        torch::nn::ReplicationPad1d m_replicationPad1d{nullptr};
        torch::nn::ReplicationPad2d m_replicationPad2d{nullptr};
        torch::nn::ReplicationPad3d m_replicationPad3d{nullptr};
        torch::nn::ZeroPad2d m_zeroPad2d{nullptr};
        torch::nn::ConstantPad1d m_constantPad1d{nullptr};
        torch::nn::ConstantPad2d m_constantPad2d{nullptr};
        torch::nn::ConstantPad3d m_constantPad3d{nullptr};
    public:
        PaddingModule(uint64_t p1 = 1, uint64_t p2 = 1, uint64_t p3 = 1, uint64_t p4 = 1, uint64_t p5 = 1, uint64_t p6 = 1, double p7 = 0.0, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_reflectionPad1d = register_module(std::to_string(this->m_id), torch::nn::ReflectionPad1d(torch::nn::ReflectionPad1dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2)})));
                    break;
                case 2:
                    this->m_reflectionPad2d = register_module(std::to_string(this->m_id), torch::nn::ReflectionPad2d(torch::nn::ReflectionPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)})));
                    break;
                case 3:
                    this->m_reflectionPad3d = register_module(std::to_string(this->m_id), torch::nn::ReflectionPad3d(torch::nn::ReflectionPad3dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4), static_cast<int64_t>(p5), static_cast<int64_t>(p6)})));
                    break;
                case 4:
                    this->m_replicationPad1d = register_module(std::to_string(this->m_id), torch::nn::ReplicationPad1d(torch::nn::ReplicationPad1dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2)})));
                    break;
                case 5:
                    this->m_replicationPad2d = register_module(std::to_string(this->m_id), torch::nn::ReplicationPad2d(torch::nn::ReplicationPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)})));
                    break;
                case 6:
                    this->m_replicationPad3d = register_module(std::to_string(this->m_id), torch::nn::ReplicationPad3d(torch::nn::ReplicationPad3dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4), static_cast<int64_t>(p5), static_cast<int64_t>(p6)})));
                    break;
                case 7:
                    this->m_zeroPad2d = register_module(std::to_string(this->m_id), torch::nn::ZeroPad2d(torch::nn::ZeroPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)})));
                    break;
                case 8:
                    this->m_constantPad1d = register_module(std::to_string(this->m_id), torch::nn::ConstantPad1d(torch::nn::ConstantPad1dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2)}, p7)));
                    break;
                case 9:
                    this->m_constantPad2d = register_module(std::to_string(this->m_id), torch::nn::ConstantPad2d(torch::nn::ConstantPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)}, p7)));
                    break;
                case 10:
                    this->m_constantPad3d = register_module(std::to_string(this->m_id), torch::nn::ConstantPad3d(torch::nn::ConstantPad3dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4), static_cast<int64_t>(p5), static_cast<int64_t>(p6)}, p7)));
                    break;
                default:
                    break;    
            }
            this->setId(7);
            this->calculateModuleParams(8, p1, p2, p3, p4, p5, p6, *(uint64_t*)&p7, type);
        }

        void replaceModule(uint64_t p1 = 1, uint64_t p2 = 1, uint64_t p3 = 1, uint64_t p4 = 1, uint64_t p5 = 1, uint64_t p6 = 1, double p7 = 0.0, uint8_t type = 1) {
            this->m_reflectionPad1d = torch::nn::ReflectionPad1d{nullptr};
            this->m_reflectionPad2d = torch::nn::ReflectionPad2d{nullptr};
            this->m_reflectionPad3d = torch::nn::ReflectionPad3d{nullptr};
            this->m_replicationPad1d = torch::nn::ReplicationPad1d{nullptr};
            this->m_replicationPad2d = torch::nn::ReplicationPad2d{nullptr};
            this->m_replicationPad3d = torch::nn::ReplicationPad3d{nullptr};
            this->m_zeroPad2d = torch::nn::ZeroPad2d{nullptr};
            this->m_constantPad1d = torch::nn::ConstantPad1d{nullptr};
            this->m_constantPad2d = torch::nn::ConstantPad2d{nullptr};
            this->m_constantPad3d = torch::nn::ConstantPad3d{nullptr};
            switch(type) {
                case 1:
                    this->m_reflectionPad1d = replace_module(std::to_string(this->m_id), torch::nn::ReflectionPad1d(torch::nn::ReflectionPad1dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2)})));
                    break;
                case 2:
                    this->m_reflectionPad2d = replace_module(std::to_string(this->m_id), torch::nn::ReflectionPad2d(torch::nn::ReflectionPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)})));
                    break;
                case 3:
                    this->m_reflectionPad3d = replace_module(std::to_string(this->m_id), torch::nn::ReflectionPad3d(torch::nn::ReflectionPad3dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4), static_cast<int64_t>(p5), static_cast<int64_t>(p6)})));
                    break;
                case 4:
                    this->m_replicationPad1d = replace_module(std::to_string(this->m_id), torch::nn::ReplicationPad1d(torch::nn::ReplicationPad1dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2)})));
                    break;
                case 5:
                    this->m_replicationPad2d = replace_module(std::to_string(this->m_id), torch::nn::ReplicationPad2d(torch::nn::ReplicationPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)})));
                    break;
                case 6:
                    this->m_replicationPad3d = replace_module(std::to_string(this->m_id), torch::nn::ReplicationPad3d(torch::nn::ReplicationPad3dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4), static_cast<int64_t>(p5), static_cast<int64_t>(p6)})));
                    break;
                case 7:
                    this->m_zeroPad2d = replace_module(std::to_string(this->m_id), torch::nn::ZeroPad2d(torch::nn::ZeroPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)})));
                    break;
                case 8:
                    this->m_constantPad1d = replace_module(std::to_string(this->m_id), torch::nn::ConstantPad1d(torch::nn::ConstantPad1dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2)}, p7)));
                    break;
                case 9:
                    this->m_constantPad2d = replace_module(std::to_string(this->m_id), torch::nn::ConstantPad2d(torch::nn::ConstantPad2dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4)}, p7)));
                    break;
                case 10:
                    this->m_constantPad3d = replace_module(std::to_string(this->m_id), torch::nn::ConstantPad3d(torch::nn::ConstantPad3dOptions({static_cast<int64_t>(p1), static_cast<int64_t>(p2), static_cast<int64_t>(p3), static_cast<int64_t>(p4), static_cast<int64_t>(p5), static_cast<int64_t>(p6)}, p7)));
                    break;
                default:
                    break;    
            }
            this->setId(7);
            this->calculateModuleParams(8, p1, p2, p3, p4, p5, p6, *(uint64_t*)&p7, type);
        }

        virtual torch::Tensor foward() {
            auto itr = this->m_inputs.begin();
            if(this->m_reflectionPad1d) {
                return this->m_reflectionPad1d(itr->second);
            } else if(this->m_reflectionPad2d) {
                return this->m_reflectionPad2d(itr->second);
            }  else if(this->m_reflectionPad3d) {
                return this->m_reflectionPad3d(itr->second);
            } else if(this->m_replicationPad1d) {
                return this->m_replicationPad1d(itr->second);
            } else if(this->m_replicationPad2d) {
                return this->m_replicationPad2d(itr->second);
            } else if(this->m_replicationPad3d) {
                return this->m_replicationPad3d(itr->second);
            } else if(this->m_zeroPad2d) {
                return this->m_zeroPad2d(itr->second);
            } else if(this->m_constantPad1d) {
                return this->m_constantPad1d(itr->second);
            } else if(this->m_constantPad2d) {
                return this->m_constantPad2d(itr->second);
            } else {
                return this->m_constantPad3d(itr->second);
            }
        }
    }; //class PaddingModule

    class NormalizationModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::BatchNorm1d m_batchNorm1d{nullptr};
        torch::nn::BatchNorm2d m_batchNorm2d{nullptr};
        torch::nn::BatchNorm3d m_batchNorm3d{nullptr};
        torch::nn::GroupNorm m_groupNorm{nullptr};
        torch::nn::InstanceNorm1d m_instanceNorm1d{nullptr};
        torch::nn::InstanceNorm2d m_instanceNorm2d{nullptr};
        torch::nn::InstanceNorm3d m_instanceNorm3d{nullptr};
        torch::nn::LayerNorm m_layerNorm{nullptr};
        torch::nn::LocalResponseNorm m_localResponseNorm{nullptr};
    public:
        NormalizationModule(uint64_t numFeatures1 = 1, uint64_t numFeatures2 = 1, double epsilon = 0.00005, double momentum = 0.1, double alpha = 0.0001, double beta = 0.75, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_batchNorm1d = register_module(std::to_string(this->m_id), torch::nn::BatchNorm1d(torch::nn::BatchNorm1dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 2:
                    this->m_batchNorm2d = register_module(std::to_string(this->m_id), torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 3:
                    this->m_batchNorm3d = register_module(std::to_string(this->m_id), torch::nn::BatchNorm3d(torch::nn::BatchNorm3dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 4:
                    this->m_groupNorm = register_module(std::to_string(this->m_id), torch::nn::GroupNorm(torch::nn::GroupNormOptions(static_cast<int64_t>(numFeatures1), static_cast<int64_t>(numFeatures2)).eps(epsilon)));
                    break;
                case 5:
                    this->m_instanceNorm1d = register_module(std::to_string(this->m_id), torch::nn::InstanceNorm1d(torch::nn::InstanceNorm1dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 6:
                    this->m_instanceNorm2d = register_module(std::to_string(this->m_id), torch::nn::InstanceNorm2d(torch::nn::InstanceNorm2dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 7:
                    this->m_instanceNorm3d = register_module(std::to_string(this->m_id), torch::nn::InstanceNorm3d(torch::nn::InstanceNorm3dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 8:
                    this->m_layerNorm = register_module(std::to_string(this->m_id), torch::nn::LayerNorm(torch::nn::LayerNormOptions({static_cast<int64_t>(numFeatures1), static_cast<int64_t>(numFeatures2)}).eps(epsilon)));
                    break;
                case 9:
                    this->m_localResponseNorm = register_module(std::to_string(this->m_id), torch::nn::LocalResponseNorm(torch::nn::LocalResponseNormOptions(static_cast<int64_t>(numFeatures1)).alpha(alpha).beta(beta)));
                    break;
                default:
                    break;
            }
            this->setId(8);
            this->calculateModuleParams(7, numFeatures1, numFeatures2, *(uint64_t*)&epsilon, *(uint64_t*)&momentum, *(uint64_t*)&alpha, *(uint64_t*)&beta, type);
        }

        void replaceModule(uint64_t numFeatures1 = 1, uint64_t numFeatures2 = 1, double epsilon = 0.00005, double momentum = 0.1, double alpha = 0.0001, double beta = 0.75, uint8_t type = 1) {
            this->m_batchNorm1d = torch::nn::BatchNorm1d{nullptr};
            this->m_batchNorm2d = torch::nn::BatchNorm2d{nullptr};
            this->m_batchNorm3d = torch::nn::BatchNorm3d{nullptr};
            this->m_groupNorm = torch::nn::GroupNorm{nullptr};
            this->m_instanceNorm1d = torch::nn::InstanceNorm1d{nullptr};
            this->m_instanceNorm2d = torch::nn::InstanceNorm2d{nullptr};
            this->m_instanceNorm3d = torch::nn::InstanceNorm3d{nullptr};
            this->m_layerNorm = torch::nn::LayerNorm{nullptr};
            this->m_localResponseNorm = torch::nn::LocalResponseNorm{nullptr};
            switch(type) {
                case 1:
                    this->m_batchNorm1d = replace_module(std::to_string(this->m_id), torch::nn::BatchNorm1d(torch::nn::BatchNorm1dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 2:
                    this->m_batchNorm2d = replace_module(std::to_string(this->m_id), torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 3:
                    this->m_batchNorm3d = replace_module(std::to_string(this->m_id), torch::nn::BatchNorm3d(torch::nn::BatchNorm3dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 4:
                    this->m_groupNorm = replace_module(std::to_string(this->m_id), torch::nn::GroupNorm(torch::nn::GroupNormOptions(static_cast<int64_t>(numFeatures1), static_cast<int64_t>(numFeatures2)).eps(epsilon)));
                    break;
                case 5:
                    this->m_instanceNorm1d = replace_module(std::to_string(this->m_id), torch::nn::InstanceNorm1d(torch::nn::InstanceNorm1dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 6:
                    this->m_instanceNorm2d = replace_module(std::to_string(this->m_id), torch::nn::InstanceNorm2d(torch::nn::InstanceNorm2dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 7:
                    this->m_instanceNorm3d = replace_module(std::to_string(this->m_id), torch::nn::InstanceNorm3d(torch::nn::InstanceNorm3dOptions(static_cast<int64_t>(numFeatures1)).eps(epsilon).momentum(momentum)));
                    break;
                case 8:
                    this->m_layerNorm = replace_module(std::to_string(this->m_id), torch::nn::LayerNorm(torch::nn::LayerNormOptions({static_cast<int64_t>(numFeatures1), static_cast<int64_t>(numFeatures2)}).eps(epsilon)));
                    break;
                case 9:
                    this->m_localResponseNorm = replace_module(std::to_string(this->m_id), torch::nn::LocalResponseNorm(torch::nn::LocalResponseNormOptions(static_cast<int64_t>(numFeatures1)).alpha(alpha).beta(beta)));
                    break;
                default:
                    break;
            }
            this->setId(8);
            this->calculateModuleParams(7, numFeatures1, numFeatures2, *(uint64_t*)&epsilon, *(uint64_t*)&momentum, *(uint64_t*)&alpha, *(uint64_t*)&beta, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_batchNorm1d) {
                return this->m_batchNorm1d(itr->second);
            } else if(this->m_batchNorm2d) {
                return this->m_batchNorm2d(itr->second);
            } else if(this->m_batchNorm3d) {
                return this->m_batchNorm3d(itr->second);
            } else if(this->m_groupNorm) {
                return this->m_groupNorm(itr->second);
            } else if(this->m_instanceNorm1d) {
                return this->m_instanceNorm1d(itr->second);
            } else if(this->m_instanceNorm2d) {
                return this->m_instanceNorm2d(itr->second);
            } else if(this->m_instanceNorm3d) {
                return this->m_instanceNorm3d(itr->second);
            } else if(this->m_layerNorm) {
                return this->m_layerNorm(itr->second);
            } else {
                return this->m_localResponseNorm(itr->second);
            }
        }
    }; //class NormalizationModule

    class DropoutModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::Dropout m_dropout{nullptr};
        torch::nn::Dropout2d m_dropout2d{nullptr};
        torch::nn::Dropout3d m_dropout3d{nullptr};
        torch::nn::AlphaDropout m_alphaDropout{nullptr};
        torch::nn::FeatureAlphaDropout m_featureAlphaDropout{nullptr};
    public:
        DropoutModule(double p = 0.5, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_dropout = register_module(std::to_string(this->m_id), torch::nn::Dropout(torch::nn::DropoutOptions().p(p).inplace(true)));
                    break;
                case 2:
                    this->m_dropout2d = register_module(std::to_string(this->m_id), torch::nn::Dropout2d(torch::nn::Dropout2dOptions().p(p).inplace(true)));
                    break;
                case 3:
                    this->m_dropout3d = register_module(std::to_string(this->m_id), torch::nn::Dropout3d(torch::nn::Dropout3dOptions().p(p).inplace(true)));
                    break;
                case 4:
                    this->m_alphaDropout = register_module(std::to_string(this->m_id), torch::nn::AlphaDropout(torch::nn::AlphaDropoutOptions().p(p).inplace(true)));
                    break;
                case 5:
                    this->m_featureAlphaDropout = register_module(std::to_string(this->m_id), torch::nn::FeatureAlphaDropout(torch::nn::FeatureAlphaDropoutOptions().p(p).inplace(true)));
                    break;
                default:
                    break;
            }
            this->setId(9);
            this->calculateModuleParams(2, *(uint64_t*)&p, type);
        }

        void replaceModule(double p = 0.5, uint8_t type = 1) {
            this->m_dropout = torch::nn::Dropout{nullptr};
            this->m_dropout2d = torch::nn::Dropout2d{nullptr};
            this->m_dropout3d = torch::nn::Dropout3d{nullptr};
            this->m_alphaDropout = torch::nn::AlphaDropout{nullptr};
            this->m_featureAlphaDropout = torch::nn::FeatureAlphaDropout{nullptr};
            switch(type) {
                case 1:
                    this->m_dropout = replace_module(std::to_string(this->m_id), torch::nn::Dropout(torch::nn::DropoutOptions().p(p).inplace(true)));
                    break;
                case 2:
                    this->m_dropout2d = replace_module(std::to_string(this->m_id), torch::nn::Dropout2d(torch::nn::Dropout2dOptions().p(p).inplace(true)));
                    break;
                case 3:
                    this->m_dropout3d = replace_module(std::to_string(this->m_id), torch::nn::Dropout3d(torch::nn::Dropout3dOptions().p(p).inplace(true)));
                    break;
                case 4:
                    this->m_alphaDropout = replace_module(std::to_string(this->m_id), torch::nn::AlphaDropout(torch::nn::AlphaDropoutOptions().p(p).inplace(true)));
                    break;
                case 5:
                    this->m_featureAlphaDropout = replace_module(std::to_string(this->m_id), torch::nn::FeatureAlphaDropout(torch::nn::FeatureAlphaDropoutOptions().p(p).inplace(true)));
                    break;
                default:
                    break;
            }
            this->setId(9);
            this->calculateModuleParams(2, *(uint64_t*)&p, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_dropout) {
                return this->m_dropout(itr->second);
            } else if(this->m_dropout2d) {
                return this->m_dropout2d(itr->second);
            } else if(this->m_dropout3d) {
                return this->m_dropout3d(itr->second);
            } else if(this->m_alphaDropout) {
                return this->m_alphaDropout(itr->second);
            } else {
                return this->m_featureAlphaDropout(itr->second);
            }
        }
    }; //class DropoutModule

    class ActivationModule : public Pagoda::GeneralDefinitions::GeneralObject {
    private:
        torch::nn::ELU m_elu{nullptr};
        torch::nn::Hardshrink m_hardShrink{nullptr};
        torch::nn::Hardtanh m_hardTanh{nullptr};
        torch::nn::LeakyReLU m_leakyRelu{nullptr};
        torch::nn::LogSigmoid m_logSigmoid{nullptr};
        torch::nn::PReLU m_prelu{nullptr};
        torch::nn::ReLU m_relu{nullptr};
        torch::nn::ReLU6 m_relu6{nullptr};
        torch::nn::RReLU m_rrelu{nullptr};
        torch::nn::SELU m_selu{nullptr};
        torch::nn::CELU m_celu{nullptr};
        torch::nn::GELU m_gelu{nullptr};
        torch::nn::Sigmoid m_sigmoid{nullptr};
        torch::nn::SiLU m_silu{nullptr};
        torch::nn::Mish m_mish{nullptr};
        torch::nn::Softplus m_softPlus{nullptr};
        torch::nn::Softshrink m_softShrink{nullptr};
        torch::nn::Softsign m_softSign{nullptr};
        torch::nn::Tanh m_tanh{nullptr};
        torch::nn::Tanhshrink m_tanhShrink{nullptr};
        torch::nn::Threshold m_threshold{nullptr};
        torch::nn::GLU m_glu{nullptr};
        torch::nn::Softmin m_softmin{nullptr};
        torch::nn::Softmax m_softmax{nullptr};
        torch::nn::Softmax2d m_softmax2d{nullptr};
        torch::nn::LogSoftmax m_logSoftmax{nullptr};
    public:
        ActivationModule(double param1 = 1.0, double param2 = 1.0, uint64_t param3 = 1, uint64_t param4 = 1, double param5 = 1.0, bool bias = true, uint8_t type = 1) {
            switch(type) {
                case 1:
                    this->m_elu = register_module(std::to_string(this->m_id), torch::nn::ELU(torch::nn::ELUOptions().alpha(param1).inplace(true)));
                    break;
                case 2:
                    this->m_hardShrink = register_module(std::to_string(this->m_id), torch::nn::Hardshrink(torch::nn::HardshrinkOptions().lambda(param1)));
                    break;
                case 3:
                    this->m_hardTanh = register_module(std::to_string(this->m_id), torch::nn::Hardtanh(torch::nn::HardtanhOptions().min_val(param1).max_val(param2).inplace(true)));
                    break;
                case 4:
                    this->m_leakyRelu = register_module(std::to_string(this->m_id), torch::nn::LeakyReLU(torch::nn::LeakyReLUOptions().negative_slope(param1).inplace(true)));
                    break;
                case 5:
                    this->m_logSigmoid = register_module(std::to_string(this->m_id), torch::nn::LogSigmoid());
                    break;
                case 6:
                    this->m_prelu = register_module(std::to_string(this->m_id), torch::nn::PReLU(torch::nn::PReLUOptions().num_parameters(param1)));
                    break;
                case 7:
                    this->m_relu = register_module(std::to_string(this->m_id), torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)));
                    break;
                case 8:
                    this->m_relu6 = register_module(std::to_string(this->m_id), torch::nn::ReLU6(torch::nn::ReLU6Options().inplace(true)));
                    break;
                case 9:
                    this->m_rrelu = register_module(std::to_string(this->m_id), torch::nn::RReLU(torch::nn::RReLUOptions().lower(param1).upper(param2).inplace(true)));
                    break;
                case 10:
                    this->m_selu = register_module(std::to_string(this->m_id), torch::nn::SELU(torch::nn::SELUOptions().inplace(true)));
                    break;
                case 11:
                    this->m_celu = register_module(std::to_string(this->m_id), torch::nn::CELU(torch::nn::CELUOptions().alpha(param1).inplace(true)));
                    break;
                case 12:
                    this->m_gelu = register_module(std::to_string(this->m_id), torch::nn::GELU());
                    break;
                case 13:
                    this->m_sigmoid = register_module(std::to_string(this->m_id), torch::nn::Sigmoid());
                    break;
                case 14:
                    this->m_silu = register_module(std::to_string(this->m_id), torch::nn::SiLU());
                    break;
                case 15:
                    this->m_mish = register_module(std::to_string(this->m_id), torch::nn::Mish());
                    break;
                case 16:
                    this->m_softPlus = register_module(std::to_string(this->m_id), torch::nn::Softplus(torch::nn::SoftplusOptions().beta(param1).threshold(param2)));
                    break;
                case 17:
                    this->m_softShrink = register_module(std::to_string(this->m_id), torch::nn::Softshrink(torch::nn::SoftshrinkOptions(param1)));
                    break;
                case 18:
                    this->m_softSign = register_module(std::to_string(this->m_id), torch::nn::Softsign());
                    break;
                case 19:
                    this->m_tanh = register_module(std::to_string(this->m_id), torch::nn::Tanh());
                    break;
                case 20:
                    this->m_tanhShrink = register_module(std::to_string(this->m_id), torch::nn::Tanhshrink());
                    break;
                case 21:
                    this->m_threshold = register_module(std::to_string(this->m_id), torch::nn::Threshold(torch::nn::ThresholdOptions(param1, param2).inplace(true)));
                    break;
                case 22:
                    this->m_glu = register_module(std::to_string(this->m_id), torch::nn::GLU(torch::nn::GLUOptions(param1)));
                    break;
                case 23:
                    this->m_softmin = register_module(std::to_string(this->m_id), torch::nn::Softmin(torch::nn::SoftminOptions(param1)));
                    break;
                case 24:
                    this->m_softmax = register_module(std::to_string(this->m_id), torch::nn::Softmax(torch::nn::SoftmaxOptions(param1)));
                    break;
                case 25:
                    this->m_softmax2d = register_module(std::to_string(this->m_id), torch::nn::Softmax2d());
                    break;
                case 26:
                    this->m_logSoftmax = register_module(std::to_string(this->m_id), torch::nn::LogSoftmax(torch::nn::LogSoftmaxOptions(param1)));
                    break;
                default:
                    break;
            }
            this->setId(10);
            this->calculateModuleParams(7, *(uint64_t*)&param1, *(uint64_t*)&param2, param3, param4, *(uint64_t*)&param5, bias, type);
        }

        void replaceModule(double param1 = 1.0, double param2 = 1.0, uint64_t param3 = 1, uint64_t param4 = 1, double param5 = 1.0, bool bias = true, uint8_t type = 1) {
            this->m_elu = torch::nn::ELU{nullptr};
            this->m_hardShrink = torch::nn::Hardshrink{nullptr};
            this->m_hardTanh = torch::nn::Hardtanh{nullptr};
            this->m_leakyRelu = torch::nn::LeakyReLU{nullptr};
            this->m_logSigmoid = torch::nn::LogSigmoid{nullptr};
            this->m_prelu = torch::nn::PReLU{nullptr};
            this->m_relu = torch::nn::ReLU{nullptr};
            this->m_relu6 = torch::nn::ReLU6{nullptr};
            this->m_rrelu = torch::nn::RReLU{nullptr};
            this->m_selu = torch::nn::SELU{nullptr};
            this->m_celu = torch::nn::CELU{nullptr};
            this->m_gelu = torch::nn::GELU{nullptr};
            this->m_sigmoid = torch::nn::Sigmoid{nullptr};
            this->m_silu = torch::nn::SiLU{nullptr};
            this->m_mish = torch::nn::Mish{nullptr};
            this->m_softPlus = torch::nn::Softplus{nullptr};
            this->m_softShrink = torch::nn::Softshrink{nullptr};
            this->m_softSign = torch::nn::Softsign{nullptr};
            this->m_tanh = torch::nn::Tanh{nullptr};
            this->m_tanhShrink = torch::nn::Tanhshrink{nullptr};
            this->m_threshold = torch::nn::Threshold{nullptr};
            this->m_glu = torch::nn::GLU{nullptr};
            this->m_softmin = torch::nn::Softmin{nullptr};
            this->m_softmax = torch::nn::Softmax{nullptr};
            this->m_softmax2d = torch::nn::Softmax2d{nullptr};
            this->m_logSoftmax = torch::nn::LogSoftmax{nullptr};

            switch(type) {
                case 1:
                    this->m_elu = replace_module(std::to_string(this->m_id), torch::nn::ELU(torch::nn::ELUOptions().alpha(param1).inplace(true)));
                    break;
                case 2:
                    this->m_hardShrink = replace_module(std::to_string(this->m_id), torch::nn::Hardshrink(torch::nn::HardshrinkOptions().lambda(param1)));
                    break;
                case 3:
                    this->m_hardTanh = replace_module(std::to_string(this->m_id), torch::nn::Hardtanh(torch::nn::HardtanhOptions().min_val(param1).max_val(param2).inplace(true)));
                    break;
                case 4:
                    this->m_leakyRelu = replace_module(std::to_string(this->m_id), torch::nn::LeakyReLU(torch::nn::LeakyReLUOptions().negative_slope(param1).inplace(true)));
                    break;
                case 5:
                    this->m_logSigmoid = replace_module(std::to_string(this->m_id), torch::nn::LogSigmoid());
                    break;
                case 6:
                    this->m_prelu = replace_module(std::to_string(this->m_id), torch::nn::PReLU(torch::nn::PReLUOptions().num_parameters(param1)));
                    break;
                case 7:
                    this->m_relu = replace_module(std::to_string(this->m_id), torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)));
                    break;
                case 8:
                    this->m_relu6 = replace_module(std::to_string(this->m_id), torch::nn::ReLU6(torch::nn::ReLU6Options().inplace(true)));
                    break;
                case 9:
                    this->m_rrelu = replace_module(std::to_string(this->m_id), torch::nn::RReLU(torch::nn::RReLUOptions().lower(param1).upper(param2).inplace(true)));
                    break;
                case 10:
                    this->m_selu = replace_module(std::to_string(this->m_id), torch::nn::SELU(torch::nn::SELUOptions().inplace(true)));
                    break;
                case 11:
                    this->m_celu = replace_module(std::to_string(this->m_id), torch::nn::CELU(torch::nn::CELUOptions().alpha(param1).inplace(true)));
                    break;
                case 12:
                    this->m_gelu = replace_module(std::to_string(this->m_id), torch::nn::GELU());
                    break;
                case 13:
                    this->m_sigmoid = replace_module(std::to_string(this->m_id), torch::nn::Sigmoid());
                    break;
                case 14:
                    this->m_silu = replace_module(std::to_string(this->m_id), torch::nn::SiLU());
                    break;
                case 15:
                    this->m_mish = replace_module(std::to_string(this->m_id), torch::nn::Mish());
                    break;
                case 16:
                    this->m_softPlus = replace_module(std::to_string(this->m_id), torch::nn::Softplus(torch::nn::SoftplusOptions().beta(param1).threshold(param2)));
                    break;
                case 17:
                    this->m_softShrink = replace_module(std::to_string(this->m_id), torch::nn::Softshrink(torch::nn::SoftshrinkOptions(param1)));
                    break;
                case 18:
                    this->m_softSign = replace_module(std::to_string(this->m_id), torch::nn::Softsign());
                    break;
                case 19:
                    this->m_tanh = replace_module(std::to_string(this->m_id), torch::nn::Tanh());
                    break;
                case 20:
                    this->m_tanhShrink = replace_module(std::to_string(this->m_id), torch::nn::Tanhshrink());
                    break;
                case 21:
                    this->m_threshold = replace_module(std::to_string(this->m_id), torch::nn::Threshold(torch::nn::ThresholdOptions(param1, param2).inplace(true)));
                    break;
                case 22:
                    this->m_glu = replace_module(std::to_string(this->m_id), torch::nn::GLU(torch::nn::GLUOptions(param1)));
                    break;
                case 23:
                    this->m_softmin = replace_module(std::to_string(this->m_id), torch::nn::Softmin(torch::nn::SoftminOptions(param1)));
                    break;
                case 24:
                    this->m_softmax = replace_module(std::to_string(this->m_id), torch::nn::Softmax(torch::nn::SoftmaxOptions(param1)));
                    break;
                case 25:
                    this->m_softmax2d = replace_module(std::to_string(this->m_id), torch::nn::Softmax2d());
                    break;
                case 26:
                    this->m_logSoftmax = replace_module(std::to_string(this->m_id), torch::nn::LogSoftmax(torch::nn::LogSoftmaxOptions(param1)));
                    break;
                default:
                    break;
            }
            this->setId(10);
            this->calculateModuleParams(7, *(uint64_t*)&param1, *(uint64_t*)&param2, param3, param4, *(uint64_t*)&param5, bias, type);
        }

        virtual torch::Tensor forward() {
            auto itr = this->m_inputs.begin();
            if(this->m_elu) {
                return this->m_elu(itr->second);
            } else if(this->m_hardShrink) {
                return this->m_hardShrink(itr->second);
            } else if(this->m_hardTanh) {
                return this->m_hardTanh(itr->second);
            } else if(this->m_leakyRelu) {
                return this->m_leakyRelu(itr->second);
            } else if(this->m_logSigmoid) {
                return this->m_logSigmoid(itr->second);
            } else if(this->m_prelu) {
                return this->m_prelu(itr->second);
            } else if(this->m_relu) {
                return this->m_relu(itr->second);
            } else if(this->m_relu6) {
                return this->m_relu6(itr->second);
            } else if(this->m_rrelu) {
                return this->m_rrelu(itr->second);
            } else if(this->m_selu) {
                return this->m_selu(itr->second);
            } else if(this->m_celu) {
                return this->m_celu(itr->second);
            } else if(this->m_gelu) {
                return this->m_gelu(itr->second);
            } else if(this->m_sigmoid) {
                return this->m_sigmoid(itr->second);
            } else if(this->m_silu) {
                return this->m_silu(itr->second);
            } else if(this->m_mish) {
                return this->m_mish(itr->second);
            } else if(this->m_softPlus) {
                return this->m_softPlus(itr->second);
            } else if(this->m_softShrink) {
                return this->m_softShrink(itr->second);
            } else if(this->m_softSign) {
                return this->m_softSign(itr->second);
            } else if(this->m_tanh) {
                return this->m_tanh(itr->second);
            } else if(this->m_tanhShrink) {
                return this->m_tanhShrink(itr->second);
            } else if(this->m_threshold) {
                return this->m_threshold(itr->second);
            } else if(this->m_glu) {
                return this->m_glu(itr->second);
            } else if(this->m_softmin) {
                return this->m_softmin(itr->second);
            } else if(this->m_softmax) {
                return this->m_softmax(itr->second);
            } else if(this->m_softmax2d) {
                return this->m_softmax2d(itr->second);
            } else {
                return this->m_logSoftmax(itr->second);
            } 
        }
    }; //class ActivationModule

    
} //namespace ModuleDefinitions
} //namepsace Pagoda