//
//  Config.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/ConfigParser.h"
#include "UKN/Resource.h"
#include "UKN/Stream.h"

#include "ConfigParserXml.h"

namespace ukn {
    
    UKN_API ConfigParserPtr MakeConfigParser(ResourcePtr resource) {
        if(*resource) {
            if(resource->getName().find(L".xml") != ukn_string::npos) {
                ConfigParserPtr ptr = MakeSharedPtr<ConfigParserXmlImpl>();
                if(ptr && ptr->open(resource)) {
                    return ptr;
                }
            }
        } else {
            return MakeSharedPtr<ConfigParserXmlImpl>();
        }
        
        return ConfigParserPtr();
    }

    UKN_API ConfigParserPtr MakeEmptyConfigParser(ConfigParserType type) {
        switch(type) {
            case CPT_XML: {
                ConfigParserPtr ptr = MakeSharedPtr<ConfigParserXmlImpl>();
                ptr->create();
                return ptr;
            }
            case CPT_JSON:
            default:
                break;
        }
        return ConfigParserPtr();
    }

    
} // namespace ukn