//
//  AutoUpdate.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/AutoUpdate.h"
#include "UKN/PreDeclare.h"
#include "mist/Module.h"

namespace ukn {
    
    using namespace mist;
    
    class AutoUpdateModule: public mist::Module {
    public:
        AutoUpdateModule() {
            // register module
            ModuleManager::Instance().addModule(this);
        }
        
        const UknString& getName() const {
            static UknString name(L"AutoUpdate");
            return name;
        }
        
        bool init() {
            return true;
        }
        
        void update() {
            UKN_FOR_EACH(AutoUpdate* item, AutoListElement<AutoUpdate>::GetList()) {
                if(item->isEnabled())
                    item->onUpdate();
            }
        }
        
        void shutdown() {
            
        }
    };
    
    static AutoUpdateModule g_autoupdate_module;
    
    AutoUpdate::AutoUpdate():
    mEnabled(true) {
        
    }
    
    AutoUpdate::~AutoUpdate() {
        
    }
    
    void AutoUpdate::enable() {
        mEnabled = true;
    }
    
    void AutoUpdate::disable() {
        mEnabled = false;
    }
    
    bool AutoUpdate::isEnabled() const {
        return mEnabled;
    }
    
    
} // namespace ukn
