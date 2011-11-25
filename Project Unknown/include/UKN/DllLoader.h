//
//  DllLoader.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_DllLoader_h
#define Project_Unknown_DllLoader_h

#include "Platform.h"

namespace ukn {
    
    class DllLoaderImpl;
    
    class UKN_API DllLoader {
    public:
        DllLoader();
        ~DllLoader();
        
        bool open(const char* path);
        void close();
        
        void* getProc(const char* name);

    private:
        DllLoaderImpl* mImpl;
    };
    
} // namespace ukn


#endif
