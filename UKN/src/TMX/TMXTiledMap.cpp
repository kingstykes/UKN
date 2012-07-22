//
//  TMXTiledMap.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/tmx/TMXTiledMap.h"
#include "UKN/SpriteBatch.h"
#include "UKN/Asset.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Operations.h"
#include "UKN/Texture.h"
#include "UKN/Color.h"
#include "UKN/ConfigParser.h"
#include "UKN/Logger.h"
#include "UKN/Base64.h"
#include "UKN/ZipUtil.h"
#include "UKN/Context.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Convert.h"

namespace ukn {
    
    namespace tmx {
        
        Map::Map():
        mMapWidth(0),
        mMapHeight(0),
        mTileWidth(0),
        mTileHeight(0) {
            mMapRenderer = Context::Instance().getGraphicFactory().createSpriteBatch();
        }
        
        Map::Map(const String& map_file):
        mMapWidth(0),
        mMapHeight(0),
        mTileWidth(0),
        mTileHeight(0) {
            mMapRenderer = Context::Instance().getGraphicFactory().createSpriteBatch();
            
            deserialize(AssetManager::Instance().load<ConfigParser>(map_file));
        }
        
        Map::~Map() {
            
        }
        
        const Map::TileSetList& Map::getTileSets() const {
            return mTileSets;
        }
        
        const Map::LayerList& Map::getLayers() const {
            return mLayers;
        }
        
        SpriteBatchPtr Map::getMapRenderer() const {
            return mMapRenderer;
        }

        void Map::setMapRenderer(SpriteBatchPtr renderer) {
            mMapRenderer = renderer;
        }
        
        MapOrientation Map::getOrientation() const {
            return mOrientation;
        }
        
        uint32 Map::getMapWidth() const {
            return mMapWidth;
        }
        
        uint32 Map::getMapHeight() const {
            return mMapHeight;
        }
        
        int32 Map::getTileWidth() const {
            return mTileWidth;
        }
        
        int32 Map::getTileHeight() const {
            return mTileHeight;
        }
        
        void Map::setPosition(const Vector2& pos) {
            mPosition = pos;
        }
        
        const Vector2& Map::getPosition() const {
            return mPosition;
        }
        
        bool Map::serialize(const ConfigParserPtr& config) {
            return false;
        }
        
        void Map::setMapViewRect(const Rectangle& vr) {
            mViewRect = vr;
        }
        
        const Rectangle& Map::getMapViewRect() const {
            return mViewRect;
        }
        
        void Map::parseProperties(PropertyContainer& cont, const ConfigParserPtr& config) {
            if(config->toNode("properties")) {
                if(config->toFirstChild()) {
                    do {
                        if(config->hasAttribute("name")) 
                            cont.properties.push_back(Property(config->getString("name"),
                                                               config->getString("value")));
                        
                    } while(config->toNextChild());
                    
                    // <properties>
                    config->toParent();
                }
                
                // 
                config->toParent();
            }
        }
        
        static void build_tileset_tiles(TileSet& tileset, uint32 tileset_id) {
            if(tileset.image) {
                int32 wcount = tileset.image->getWidth() / tileset.tile_width;
                int32 hcount = tileset.image->getHeight() / tileset.tile_height;
                
                tileset.tiles.resize(wcount * hcount);
                //ukn_logged_assert(tileset.tiles, "error allocing tileset tiles");
                
                for(int32 i = 0; i < wcount * hcount; ++i) {
                    Tile& tile = tileset.tiles[i];
                    
                    tile.tile_id = i;
                    
                    int32 x = (i % wcount) * (tileset.tile_width + tileset.spacing);
                    int32 y = (i / wcount) * (tileset.tile_height + tileset.spacing);
                    tile.tile_texture_rect = Rectangle(x + tileset.margin,
                                                       y + tileset.margin,
                                                       x + tileset.tile_width + tileset.margin,
                                                       y + tileset.tile_height + tileset.margin);
                    
                    tile.tileset_id = tileset_id;
                }
            }
        }
        
        void Map::deserialize_tile_set(TileSet& ts, uint32 ts_id, const ConfigParserPtr& config) {
            ts.name = config->getString("name");
            ts.tile_width = config->getInt("tilewidth");
            ts.tile_height = config->getInt("tileheight");
            ts.margin = config->getInt("margin");
            ts.spacing = config->getInt("spacing");
            
            if(config->toNode("tileoffset")) {
                ts.tile_offset_x = config->getInt("x");
                ts.tile_offset_y = config->getInt("y");
                config->toParent();
            }
            if(config->toNode("image")) {
                ts.image = AssetManager::Instance().load<Texture>(String::GetFilePath(config->getName()) + String::StringToWString(config->getString("source")));
                config->toParent();
            }
            parseProperties(ts.property, config);
            
            build_tileset_tiles(ts, ts_id);
            
            // parser tile properties
            if(config->toFirstChild()) {
                do {
                    if(config->getCurrentNodeName() == "tile") {
                        int32 tileid = config->getInt("id");
                        ukn_assert(tileid < ts.tiles.size());
                        
                        Tile* tile = ts.tileAt(tileid);
                        if(tile) {
                            parseProperties(tile->property, config);
                        }
                    }
                } while(config->toNextChild());
                
                // <tileset>
                config->toParent();
            }
        }
        
        
        void Map::parseLayer(const ConfigParserPtr& config) {
            
            static const int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
            static const int FLIPPED_VERTICALLY_FLAG   = 0x40000000;
            static const int FLIPPED_DIAGONALLY_FLAG   = 0x20000000;
            
            ukn_assert(!mTileSets.empty());
            
            mLayers.push_back(new Layer());
            
            Layer& layer = *mLayers.back();
            layer.name = config->getString("name");
            layer.width = config->getInt("width");
            layer.height = config->getInt("height");
            layer.x = config->getInt("x");
            layer.y = config->getInt("y");
            layer.visible = config->getBool("visible", true);
            layer.opacity = config->getFloat("opacity", 1.0f);
            
            if(config->toNode("data")) {
                ukn_string encoding = config->getString("encoding");
                ukn_string compression = config->getString("compression");
                
                if(encoding == "base64") {
                    ukn_string str_data = config->getString(ukn_string());
                    ukn_assert(!str_data.empty());
                    
                    ukn_string::iterator begin = str_data.begin();
                    while(*begin == '\n' || *begin == ' ')
                        begin++;
                    ukn_string::iterator end = str_data.end();
                    str_data.erase(str_data.begin(), begin);
                    
                    // base64 decode
                    Array<uint8> data(base64_decode(/* node value */
                                                    str_data
                                                    ));
                    ukn_assert(data.size() != 0);
                    
                    // zlib/gzip decompress
                    Array<uint8> dp_data(zlib_decompress(data.begin(), (uint32)data.size()));
                    
                    ukn_assert(dp_data.size() == layer.width * layer.height * 4);
                    
                    uint32 tile_index = 0;
                    uint8* raw_data = (uint8*)dp_data.begin();
                    
                    layer.tiles.resize(layer.width * layer.height);
                    
                    layer.parent = this;
                    
                    for(int32 j = 0; j < layer.height; ++j) {
                        for(int32 i = 0; i < layer.width; ++i) {
                            // for tmx format wiki
                            uint32 global_tile_id = raw_data[tile_index] |
                            raw_data[tile_index + 1] << 8 |
                            raw_data[tile_index + 2] << 16 |
                            raw_data[tile_index + 3] << 24;
                            tile_index += 4;
                            
                            // Read out the flags
                            /* Bit 32 is used for storing whether the tile is horizontally flipped and bit 31 is used for the vertically flipped tiles. And since Tiled Qt 0.8.0, bit 30 means whether the tile is flipped (anti) diagonally, enabling tile rotation
                             */
                            bool flipped_horizontally = global_tile_id & FLIPPED_HORIZONTALLY_FLAG;
                            bool flipped_vertically = global_tile_id & FLIPPED_VERTICALLY_FLAG;                                          
                            bool flipped_diagonally = global_tile_id & FLIPPED_DIAGONALLY_FLAG;
                            
                            global_tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG |
                                                FLIPPED_VERTICALLY_FLAG |
                                                FLIPPED_DIAGONALLY_FLAG);
                            
                            ukn_assert(global_tile_id < mTileSets.back().first_grid + mTileSets.back().tiles.size());
                            
                            // Resolve the tile
                            if(global_tile_id == 0) {
                                layer.tiles[i + j * layer.height].tile_id = -1;
                            } else {
                                Tile* g_tile = getTileWithGid(global_tile_id);
                                if(g_tile) {
                                    Tile& tile = layer.tiles[i + j * layer.width];
                                    
                                    tile = *g_tile;
                                    
                                    tile.flipped_diagonally = flipped_diagonally;
                                    tile.flipped_horizontally = flipped_horizontally;
                                    tile.flipped_vertically = flipped_vertically;
                                    
                                    TileSet& tileset = mTileSets[tile.tileset_id];
                                    tile.tile_bounding_rect = Rectangle((i % layer.width) * tileset.tile_width,
                                                                        (i / layer.width) * tileset.tile_height,
                                                                        (i % layer.width) * tileset.tile_width + tileset.tile_width,
                                                                        (i / layer.width) * tileset.tile_height + tileset.tile_height);
                                } else
                                    log_error("ukn::tmx::Map::parseLayer: invalid tile with gid " + Convert::ToString(global_tile_id));
                            }
                        }
                    }
                    
                } else if(encoding == "csv") {
                    StringTokenlizer tiles(config->getString(ukn_string()), ",");
                    ukn_assert(tiles.size() == layer.width * layer.height);
                    for(int32 j = 0; j < layer.height; ++j) {
                        for(int32 i = 0; i < layer.width; ++i) {
                            Tile* g_tile = getTileWithGid(Convert::ToInt32(tiles[j * layer.width + i]));
                            if(g_tile) {
                                Tile& tile = layer.tiles[i + j * layer.width];
                                
                                tile = *g_tile;
                                
                                TileSet& tileset = mTileSets[tile.tileset_id];
                                tile.tile_bounding_rect = Rectangle((i % layer.width) * tileset.tile_width,
                                                                    (i / layer.width) * tileset.tile_height,
                                                                    (i % layer.width) * tileset.tile_width + tileset.tile_width,
                                                                    (i / layer.width) * tileset.tile_height + tileset.tile_height);
                            } else
                                log_error("ukn::tmx::Map::parseLayer: invalid tile with gid " + tiles[j * layer.width + i]);
                        }
                    }
                }
                
                // <layer>
                config->toParent();
            }
            
            parseProperties(layer.property, config);
        }
        
        Tile* Map::getTileWithGid(int32 gid) {
            for (int32 ti = (int32)mTileSets.size() - 1; ti >= 0; --ti) {
                TileSet& tileset = mTileSets[ti];
                if (tileset.first_grid <= gid) {
                    return &tileset.tiles[gid - tileset.first_grid];
                }
            }
            return 0;
        }
        
        void Map::parseTileset(const ConfigParserPtr& config) {
            ukn_string source = config->getString("source");
            
            mTileSets.push_back(TileSet());
            
            TileSet& ts = mTileSets.back();
            ts.first_grid = config->getInt("firstgid");
            
            if(source.empty()) {
                deserialize_tile_set(ts, 
                                     (uint32)mTileSets.size() - 1, 
                                     config);
            } else {
                // external tile set file
                ConfigParserPtr extern_config = AssetManager::Instance().load<ConfigParser>(String::GetFilePath(config->getName()) + String::StringToWString(source));
                if(extern_config->toNode("tileset")) {
                    deserialize_tile_set(ts, 
                                         (uint32)mTileSets.size() - 1, 
                                         extern_config);
                } else {
                    log_error("ukn::tmx::Map::deserialize: invalid external config file");
                }
            }
        }
        
        void Map::parseObjectGroup(const ConfigParserPtr& config) {
            mLayers.push_back(new ObjectGroup());
            
            ObjectGroup& obj_group = *static_cast<ObjectGroup*>(mLayers.back().get());
            
            // layer properties
            obj_group.name = config->getString("name");
            obj_group.width = config->getInt("width");
            obj_group.height = config->getInt("height");
            obj_group.x = config->getInt("x");
            obj_group.y = config->getInt("y");
            obj_group.visible = config->getBool("visible", true);
            obj_group.opacity = config->getFloat("opacity", 1.0f);
            
            parseProperties(obj_group.property, config);
            
            if(config->toFirstChild()) {
                do {
                    ukn_string node_type = config->getCurrentNodeName();
                    if(node_type == "object") {
                        Object obj;
                        
                        obj.name = config->getString("name");
                        obj.type = config->getString("type");
                        
                        obj.x = config->getInt("x");
                        obj.y = config->getInt("y");
                        obj.width = config->getInt("width");
                        obj.height = config->getInt("height");
                        
                        obj.gid = config->getInt("gid", -1);
                        if(obj.gid != -1) {
                            obj.tile = *getTileWithGid(obj.gid);
                        } else {
                            obj.image = AssetManager::Instance().load<Texture>(String::GetFilePath(config->getName()) + String::StringToWString(config->getString("image")));
                        }
                        
                        parseProperties(obj.property, config);
                        
                        if(config->toNode("polygon")) {
                            ukn_logged_assert(false, "ukn::tmx::Map: polygon object not supported");
                            config->toParent();
                        } else if(config->toNode("polyline")) {
                            ukn_logged_assert(false, "ukn::tmx::Map: polyline object not supported");
                            config->toParent();
                        }
                    }
                    
                } while(config->toNextChild());
            }
        }
        
        bool Map::deserialize(const ConfigParserPtr& config) {
            if(config->toNode("map")) {
                ukn_string orientation = config->getString("orientation", "orthogonall");
                if(orientation == "orthogonal")
                    mOrientation = MO_Orthogonal;
                else
                    mOrientation = MO_Isometric;
                
                mMapWidth = config->getInt("width");
                mMapHeight = config->getInt("height");
                mTileWidth = config->getInt("tilewidth");
                mTileHeight = config->getInt("tileheight");
                
                if(config->toFirstChild()) {
                    do {
                        ukn_string node_type = config->getCurrentNodeName();
                        if(node_type == "tileset") {                            
                            parseTileset(config);
    
                        } else if(node_type == "layer") {
                            parseLayer(config);
    
                        } else if(node_type == "objectgroup") {
                            parseObjectGroup(config);
                        }
                        
                    } while(config->toNextChild());
                    
                    mViewRect.set(Vector2(0, 0), 
                                  Vector2(mMapWidth * mTileWidth,
                                          mMapHeight * mTileHeight));
                    
                    // <map>
                    config->toParent();
                    return true;
                }
            }
            return false;
        }
        
        Tile& Layer::getTileAt(const Vector2& pos) {
            return tiles[pos.x / parent->getTileWidth() + pos.y / parent->getTileHeight() * parent->getMapWidth()];
        }
        
        Tile& Map::getTileAt(uint32 layer_index, const Vector2& pos) {
            return mLayers[layer_index]->getTileAt(pos);
        }
        
        const String& Map::getName() const {
            return mName;
        }
        
        Box Map::getBound() const {
            return Box(Vector3(0.f, 0.f, 0.f),
                       Vector3(mMapWidth * mTileWidth,
                               mMapHeight * mTileHeight,
                               0.f));
        }
        
        RenderBufferPtr Map::getRenderBuffer() const {
            return mMapRenderer->getRenderBuffer();
        }
        
        void Map::onRenderBegin() {
            mMapRenderer->onRenderBegin();
        }
        
        void Map::onRenderEnd() {
            mMapRenderer->onRenderEnd();
        }
        
        void Map::orthogonalRender() {
            UKN_ENUMERABLE_FOREACH(const SharedPtr<Layer>&, layer_ptr, mLayers) {
                Layer& layer = *layer_ptr;
                if(layer.visible) {
                    mMapRenderer->startBatch();
                    
                    int32 startx = 0;
                    int32 starty = 0;
                    
                    int32 endx = layer.width;
                    int32 endy = layer.height;
                    
                    startx = UKN_MAX(mViewRect.left() / mTileWidth, 0);
                    starty = UKN_MAX(mViewRect.top() / mTileHeight, 0);
                    
                    endx = UKN_MIN(mViewRect.right() / mTileWidth + 1, endx);
                    endy = UKN_MIN(mViewRect.bottom() / mTileHeight + 1, endy);
                    
                    for(int32 y = starty; y < endy; ++y) {
                        for(int32 x = startx; x < endx; ++x) {
                            Tile& tile = layer.tiles[x + y * layer.width];
                            TileSet& ts = mTileSets[tile.tileset_id];

                            if(tile.tile_id != -1) {
                                float dx = ts.tile_offset_x + x * mTileWidth;
                                float dy = ts.tile_offset_y + (y + 1) * mTileHeight - ts.tile_height;
                                
                                if(tile.flipped_diagonally) {
                                    dy += ts.tile_height - ts.tile_width;
                                }
                                if(tile.flipped_horizontally) {
                                    dx += tile.flipped_diagonally ? ts.tile_height : ts.tile_width;
                                }
                                if(tile.flipped_vertically) {
                                    dy += tile.flipped_diagonally ? ts.tile_width : ts.tile_height;
                                }
                                
                                mMapRenderer->draw(ts.image,
                                                   Vector2(dx, dy),
                                                   tile.tile_texture_rect,
                                                   Vector2(0,
                                                           0),
                                                   0.f,
                                                   Vector2(1.f, 1.f),
                                                   color::White * layer.opacity);
                                
                            }

                        }
                    }
                    
                    mMapRenderer->endBatch();
                }
            }
        }
        
        float2 Map::pixelToTileCoords(float x, float y) const {
            if(mOrientation == MO_Isometric) {
                float ratio = mTileWidth / mTileHeight;
                
                x -= mMapHeight * mTileWidth / 2;
                float mx = y + (x / ratio);
                float my = y - (x / ratio);
                
                return float2(mx / mTileHeight,
                              my / mTileHeight);
            } else {
                return float2(x / mTileWidth,
                              y / mTileHeight);
            }
        }
        
        float2 Map::tileToPixelCoords(float x, float y) const {
            if(mOrientation == MO_Isometric) {
                int origx = mMapHeight * mTileWidth / 2;
                return float2((x - y) * mTileWidth / 2 + origx,
                              (x + y) * mTileHeight / 2);
            } else {
                return float2(x * mTileWidth,
                              y * mTileHeight);
            }
        }
                
        void Map::isometricRender() {
            UKN_ENUMERABLE_FOREACH(const SharedPtr<Layer>&, layer_ptr, mLayers) {
                Layer& layer = *layer_ptr;
                if(layer.visible) {
                    mMapRenderer->startBatch();
                    
                    Rectangle vr = mViewRect;
                    vr.set(mViewRect.x(),
                           mViewRect.y(),
                           mViewRect.right(),
                           mViewRect.bottom());
            
                    float2 tilePos = pixelToTileCoords(vr.x(), vr.y());
                    
                    int32 rowx = (int32)std::floor(tilePos[0]);
                    int32 rowy = (int32)std::floor(tilePos[1]);
                    
                    float2 startPos = tileToPixelCoords(rowx, rowy);
                    
                    float startx = startPos[0] - mTileWidth / 2;
                    float starty = startPos[1] + mTileHeight;
                    
                    bool isUpperHalf = starty - vr.y() > mTileHeight / 2;
                    bool isLeftHalf = vr.x() - startx < mTileWidth / 2;
                    
                    if(isUpperHalf) {
                        if(isLeftHalf) {
                            rowx--;
                            startx -= mTileWidth / 2;
                        } else {
                            rowy--;
                            startx += mTileWidth / 2;
                        }
                        starty -= mTileHeight / 2;
                    }
                                        
                    bool shifted = isUpperHalf ^ isLeftHalf;
                    
                    for(float y = starty; y - mTileHeight < vr.bottom(); y += mTileHeight / 2) {
                        int32 colx = rowx;
                        int32 coly = rowy;
                        
                        for(float x = startx; x < vr.right(); x += mTileWidth) {
                            if(colx >= 0 && colx < layer.width && coly >= 0 && coly < layer.height) {
                                Tile& tile = layer.tiles[colx + coly * layer.width];

                                if(tile.tile_id != -1) {
                                    TileSet& ts = mTileSets[tile.tileset_id];
                                    
                                    float dx = ts.tile_offset_x + x;
                                    float dy = ts.tile_offset_y + y - ts.tile_height;
                                    
                                    if(tile.flipped_diagonally) {
                                        dy += ts.tile_height - ts.tile_width;
                                    }
                                    if(tile.flipped_horizontally) {
                                        dx += tile.flipped_diagonally ? ts.tile_height : ts.tile_width;
                                    }
                                    if(tile.flipped_vertically) {
                                        dy += tile.flipped_diagonally ? ts.tile_width : ts.tile_height;
                                    }
                                    
                                    mMapRenderer->draw(ts.image,
                                                       Vector2(dx,
                                                               dy),
                                                       tile.tile_texture_rect,
                                                       Vector2(0, 0),
                                                       0.f,
                                                       Vector2(1.f, 1.f),
                                                       color::White * layer.opacity);
                                }
                            }
                            
                            colx++;
                            coly--;
                        }
                        
                        if(!shifted) {
                            rowx++;
                            startx += mTileWidth/2;
                            shifted = true;
                        } else {
                            rowy++;
                            startx -= mTileWidth/2;
                            shifted = false;
                        }
                    }
                    
                    mMapRenderer->endBatch();
                }
            }
        }
        
        void Map::render() {
            mMapRenderer->begin(SBS_Deffered, Matrix4::TransMat(mPosition.x, mPosition.y, 0.f));
            
            if(mOrientation == MO_Orthogonal)
                orthogonalRender();
            else
                isometricRender();
            
            mMapRenderer->end();
        }
        
    } // namespace tmx
    
} // namespace ukn