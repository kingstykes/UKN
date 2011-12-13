//
//  Asset.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Asset.h"
#include "UKN/Font.h"
#include "UKN/Texture.h"
#include "UKN/ConfigParser.h"
#include "UKN/Resource.h"
#include "UKN/Stream.h"
#include "UKN/StringUtil.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/Logger.h"

namespace ukn {
    
    AssetManager& AssetManager::Instance() {
        static AssetManager instance;
        return instance;
    }
    
    AssetManager::AssetManager() {
        
    }
    
    AssetManager::~AssetManager() {
        
    }
    
    const AssetManager::AssetNameMap& AssetManager::getAssets() const {
        return mAssetMap;
    }
    
    void AssetManager::add(const ukn_wstring& name, const ukn_wstring& path, AssetType type) {
        mAssetMap.insert(std::make_pair(name, AssetInfo(type, name, path)));
    }
    
    template<>
    SharedPtr<Font> AssetManager::load(const ukn_wstring& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end() && it->second.type == AT_Font) {
            ResourcePtr resource = ResourceLoader::Instance().loadResource(it->second.fullPath);
            
            if(resource) {
                SharedPtr<Font> font = new Font();
                if(font && font->loadFromResource(resource))
                    return font;
            }
        }
        return SharedPtr<Font>();
    }
    
    template<>
    SharedPtr<Texture> AssetManager::load(const ukn_wstring& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end() && it->second.type == AT_Texture2D) {
            ResourcePtr resource = ResourceLoader::Instance().loadResource(it->second.fullPath);
            
            if(resource) {
                return Context::Instance().getGraphicFactory().load2DTexture(resource);
            }
        }
        return SharedPtr<Texture>();
    }
    
    template<>
    SharedPtr<ConfigParser> AssetManager::load(const ukn_wstring& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end() && it->second.type == AT_Config) {
            ResourcePtr resource = ResourceLoader::Instance().loadResource(it->second.fullPath);
            
            if(resource) {
                return MakeConfigParser(resource); 
            }
        }
        return SharedPtr<ConfigParser>();
    }
    
    template<>
    SharedPtr<Resource> AssetManager::load(const ukn_wstring& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end() && it->second.type == AT_Config) {
            ResourcePtr resource = ResourceLoader::Instance().loadResource(it->second.fullPath);
            
            if(resource) {
                return resource;
            }
        }
        return SharedPtr<Resource>();
    }
    
    ukn_string AssetManager::AssetTypeToString(AssetType type) {
        switch(type) {
            case AT_Font:       return "font"; break;
            case AT_Texture2D:  return "texture2d"; break;
            case AT_Config:     return "config"; break;
            case AT_Raw:        return "raw"; break;
            default:
                return ukn_string();
        }
    }
    
    AssetType AssetManager::StringToAssetType(const ukn_string& name) {
        if(name == "font")
            return AT_Font;
        if(name == "texture2d")
            return AT_Texture2D;
        if(name == "config")
            return AT_Config;
        if(name == "raw")
            return AT_Raw;
        return AT_Unknown;
    }
    
    void AssetManager::serialize(const ConfigParserPtr& config) {        
        if(config) {
            config->beginNode("assets");
            
            AssetNameMap::const_iterator it = mAssetMap.begin();
            while(it != mAssetMap.end()) {
                
                config->beginNode("resource");
                config->setString("name", wstring_to_string(it->first));
                config->setString("path", wstring_to_string(it->second.fullPath));
                config->setString("type", AssetTypeToString(it->second.type));
                config->endNode();
 
                ++it;
            }
            
            config->endNode();
        } else {
            log_error("ukn::AssetManager::serialize: unable to serialize asset manager, invalid config ptr");
        }
    }
    
    void AssetManager::unserialize(const ConfigParserPtr& config) {
        if(config && config->toNode("/assets")) {
            if(config->toFirstChild()) {
                do {
                    AssetInfo info;
                    
                    info.type = StringToAssetType(config->getString("type"));
                    if(info.type != AT_Unknown) {
                        info.name = string_to_wstring(config->getString("name"));
                        info.fullPath = string_to_wstring(config->getString("path"));
                        
                        if(!info.name.empty() && 
                           !info.fullPath.empty()) {
                            mAssetMap.insert(std::make_pair(info.name, info));
                        }
                    }
                } while( config->toNextChild() );
            }
        } else {
            log_error("ukn::AssetManager::unserialize: unable to unserialize asset manager, invalid config state");
        }
    }
    
} // namespace ukn