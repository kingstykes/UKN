//
//  GraphicFactory.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicFactory_h
#define Project_Unknown_GraphicFactory_h

#include "mist/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    class UKN_API GraphicFactory {
    public:
        static GraphicFactoryPtr NullObject();
        
        GraphicFactory();
        virtual ~GraphicFactory();
        
        virtual GraphicDevice& getGraphicDevice() const = 0;
        
        virtual SpriteBatchPtr createSpriteBatch() const;
        
        
        virtual GraphicBufferPtr createVertexBuffer(GraphicBuffer::Access, 
                                                    GraphicBuffer::Usage, 
                                                    uint32 count, 
                                                    const void* initialData, 
                                                    const VertexFormat& format) const = 0;
        
        virtual GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                                   GraphicBuffer::Usage, 
                                                   uint32 count, 
                                                   const void* initialData) const = 0;
        
        
        virtual RenderBufferPtr createRenderBuffer() const = 0;
        
        // FBOs
        virtual RenderViewPtr create2DRenderView(TexturePtr texture) const = 0;
        virtual RenderViewPtr create2DDepthStencilView(TexturePtr texture) const = 0;
        
        // FrameBuffer
        virtual FrameBufferPtr createFrameBuffer() const = 0;
        
        // textures
        virtual TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const = 0;
        virtual TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const = 0;
        
        // shaders & effects
        virtual EffectPtr loadEffect(const ResourcePtr& rsrc) const = 0;
        
        template<typename T>
        SharedPtr<MemoryGraphicBuffer<T> > createMemoryVertexBuffer(uint32 count,
                                                                    const T* initialData) const {
            return new MemoryGraphicBuffer<T>(count, initialData);
        }
    };
    
} // namespace ukn


#endif
