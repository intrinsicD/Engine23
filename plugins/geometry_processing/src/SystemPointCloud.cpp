//
// Created by alex on 05.02.24.
//

#include "SystemPointCloud.h"
#include "Engine.h"
#include "Events.h"
#include "components/Entity.h"
#include "Commands.h"
#include "imgui.h"
#include "components/Picker.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemPointCloudInternal{

    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemPointCloudInternal{

    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    std::string SystemPointCloud::name() {
        return "PointCloud";
    }

    void SystemPointCloud::pre_init(){

    }

    void SystemPointCloud::init(){
        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemPointCloud::remove(){
        Log::Info(name() + ": Removed").enqueue();
    }

    void ComponentGui<PointCloud>::Show(entt::entity entity_id) {
        auto &picker = Engine::Context().get<Picker>();
        auto entity = Entity(Engine::State(), picker.id.entity);
        if(!entity.is_valid()) return;

    }
}