//
//  GraphicFactory.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicFactory_h
#define Project_Unknown_GraphicFactory_h

#include "Platform.h"
#include "PreDeclare.h"
#include "GraphicBuffer.h"

namespace ukn {
    
    class UKN_API GraphicFactory {
    public:
        static GraphicFactoryPtr NullObject();
        
        virtual GraphicBufferPtr createVertexBuffer(GraphicBuffer::Usage, 
                                                    GraphicBuffer::Access, 
                                                    uint32 count, 
                                                    const void* initialData, 
                                                    const VertexFormat& format) = 0;
        
        virtual GraphicBufferPtr createIndexBuffer(GraphicBuffer::Usage, 
                                                   GraphicBuffer::Access, 
                                                   uint32 count, 
                                                   const void* initialData);
        
        
        virtual RenderBufferPtr createRenderBuffer() = 0;
        
        // FBOs
        // todo with more format, 3d render view with 3d textures etc
        virtual RenderViewPtr create2DRenderView(TexturePtr texture) = 0;
        virtual RenderViewPtr create2DDepthStencilView(TexturePtr texture) = 0;
        
        // textures
        virtual TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format);
        
        virtual TexturePtr loadTexture(const ukn_string& name, bool generateMipmaps=false) = 0; 
    };
    
} // namespace ukn


#endif
