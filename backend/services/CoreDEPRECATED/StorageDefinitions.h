#pragma once

#include "GeneralDefinitions.h"

namespace Pagoda {
namespace StorageDefinitions {

    class StorageInterface {
    private:

    public:
        std::shared_ptr<Pagoda::GeneralDefinitions::GeneralObject> getObject(uint32_t p_id) {
            return nullptr; 
            //replace this with a call to a database service to pull custom modules
        }
    }; //class StorageInterface

} //namespace StorageDefinitions
} //namespace Pagoda