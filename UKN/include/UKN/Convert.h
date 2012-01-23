//
//  Convert.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_Convert_h
#define Project_Unknown_Convert_h

#include "UKN/Platform.h"
#include "UKN/Util.h"

namespace ukn {
    
    class Convert {
    public:
        template<typename T>
        static ukn_string ToString(T t);
        
        static int16    ToInt16(const ukn_string& str);
        static int32    ToInt32(const ukn_string& str);
        static int64    ToInt64(const ukn_string& str);
        
        static uint16   ToUInt16(const ukn_string& str);
        static uint32   ToUInt32(const ukn_string& str);
        static uint64   ToUInt64(const ukn_string& str);
        
        static uint8    ToByte(const ukn_string& str);
        static double   ToDouble(const ukn_string& str);
        
        static Array<uint16> ToUnicode(const ukn_string& str);
    };
    
} // namespace ukn


#endif
