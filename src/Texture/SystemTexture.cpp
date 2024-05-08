//
// Created by alex on 08.05.24.
//

#include "SystemTexture.h"
#include "Engine.h"
#include "Components.h"
#include "Texture.h"
#include "Commands.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemTextureInternal{
        void on_
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemTextureInternal{

    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg{
    std::string SystemTexture::name(){
        return "System" + component_name();
    }

    std::string SystemTexture::component_name(){
        return "Texture";
    }

    void SystemTexture::pre_init(){

    }

    void SystemTexture::init(){
        Log::Info("Initialized", name());
    }

    void SystemTexture::remove(){
        Log::Info("Removed", name());
    }
}