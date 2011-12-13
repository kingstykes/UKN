//
//  Profiler.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Profiler.h"
#include "UKN/Common.h"

namespace ukn {
    
    namespace detail {
        
        Profile::Profile(const ukn_string& n):
        ts(Timestamp()), 
        name(n) {
            
        }
        
        Profile::~Profile() {
            Profiler::Instance().record(name, ts.elapsed());
        }
        
    } // namespace detail
    
    ukn_string ProfileData::toFormattedString() const {
        ukn_string buffer("Profile "+name+": ");
        buffer += format_string("average time %u, max time %u, min time %u, recorded frames %u, average time ratio %.3f",
                                average_time,
                                max_time,
                                min_time,
                                recored_frames,
                                time_ratio);
        return buffer;
    }
    
    Profiler& Profiler::Instance() {
        static Profiler instance;
        return instance;
    }
    
    ProfileData Profiler::get(const ukn_string& name) const {
        ProfileDataMap::const_iterator it = mProfiles.find(name);
        if(it != mProfiles.end()) {
            return it->second;
        }
        return ProfileData();
    }
    
    void Profiler::record(const ukn_string& name, uint64 time) {
        ProfileDataMap::iterator it = mProfiles.find(name);
        if(it != mProfiles.end()) {
            it->second.recored_frames++;
            
            it->second.time = time;
            it->second.average_time = (it->second.average_time * (it->second.recored_frames - 1) + time) / it->second.recored_frames;
            it->second.max_time = time > it->second.max_time ? time : it->second.max_time;
            it->second.min_time = time < it->second.min_time ? time : it->second.min_time;
            
        } else {
            ProfileData data;
            data.time = data.average_time = time;
            data.max_time = data.min_time = time;
            data.recored_frames = 1;
            data.time_ratio = 0.f;
            data.name = name;
            
            mProfiles.insert(std::make_pair(name, data));
        }
    }
    
    void Profiler::updateTimeRatio(uint64 frametime) {
        ProfileDataMap::iterator it = mProfiles.begin();
        while(it != mProfiles.end()) {
            it->second.time_ratio = (it->second.time_ratio * (it->second.recored_frames - 1) + (double)it->second.time / frametime) / it->second.recored_frames;
            
            ++it;
        }
    }
    
} // namespace ukn
