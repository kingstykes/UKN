//
//  GLConvert.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLConvert_h
#define Project_Unknown_GLConvert_h

#include "GLPreq.h"

#include "UKN/GraphicSettings.h"
#include "UKN/Vertex.h"

namespace ukn {

    inline GLenum render_mode_to_gl_mode(RenderMode mode) {
        switch (mode) {
            case RM_Point:              return GL_POINTS;
			case RM_Line:               return GL_LINES;
            case RM_LineLoop:           return GL_LINE_LOOP;
			case RM_Triangle:           return GL_TRIANGLES;
			case RM_TriangleFan:        return GL_TRIANGLE_FAN;
			case RM_TriangleStrip:      return GL_TRIANGLE_STRIP;
		}
		return GL_INVALID_VALUE;
    }
    
    inline GLenum element_format_to_gl_format(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return GL_RGBA8;
            case EF_RGBA4444: return GL_RGBA4;
            case EF_RGB565: return GL_RGB5;
            case EF_RGB5A1: return GL_RGB5_A1;
			case EF_D32: return GL_DEPTH_COMPONENT32;
			case EF_D16: return GL_DEPTH_COMPONENT16;
            case EF_D24S8: return GL_DEPTH24_STENCIL8;
                // not used in GL, total_size / component_size
            case EF_Float4:
            case EF_Float3:
            case EF_Float2:
            case EF_Float:
                return GL_FLOAT;
            case EF_UInt32: return GL_RGBA8;
            default:
                return GL_UNSIGNED_BYTE;
        }
		return GL_UNSIGNED_BYTE;
    }
    
    inline GLenum element_format_to_gl_element_type(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return GL_UNSIGNED_BYTE;
            case EF_RGBA4444: return GL_UNSIGNED_SHORT_4_4_4_4;
            case EF_RGB565: return GL_UNSIGNED_SHORT_5_6_5;
            case EF_RGB5A1: return GL_UNSIGNED_SHORT_5_5_5_1;
            case EF_D32: return GL_UNSIGNED_INT;
            case EF_D16: return GL_UNSIGNED_SHORT;
            case EF_D24S8: return GL_UNSIGNED_INT_24_8;
            case EF_UInt32: return GL_UNSIGNED_INT;
            case EF_Float:
            case EF_Float2:
            case EF_Float3:
            case EF_Float4:
                return GL_FLOAT;
            default:
                return GL_UNSIGNED_BYTE;
        }
		return GL_UNSIGNED_BYTE;
    }
    
#if !defined(UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE)
    inline GLenum vertex_usage_to_gl_array(VertexUsage usage) {
        switch(usage) {
        case VU_Position: return GL_VERTEX_ARRAY;
        case VU_UV: return GL_TEXTURE_COORD_ARRAY;
        case VU_Diffuse: return GL_COLOR_ARRAY;
        case VU_Specular: return GL_SECONDARY_COLOR_ARRAY;
        case VU_Normal: return GL_NORMAL_ARRAY;
        case VU_Tangent: return GL_TANGENT_ARRAY_EXT;
        case VU_Binormal: return GL_BINORMAL_ARRAY_EXT;
            default:
                // not in gl, in cg instead, see vertex_usage_to_attribute_location
                return GL_VERTEX_ARRAY;
        }
    }
#endif
    
    inline GLenum render_state_to_gl_state(RenderStateType type) {
        switch(type) {
            case RS_TextureWrap0:
                return GL_TEXTURE_WRAP_S;
            case RS_TextureWrap1:
                return GL_TEXTURE_WRAP_T;
            case RS_TextureWrap2:
                return GL_TEXTURE_WRAP_R;
                
            case RS_Filter:
                return type;
                
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
            case RS_ColorOp:
                return GL_TEXTURE_ENV;
#endif
                
            case RS_StencilOp:
                return type;
            case RS_StencilFunc:
                return GL_STENCIL_FUNC;
                
            case RS_DepthOp:
            case RS_DepthMask:
                return type;
                
            case RS_SrcBlend:
                return GL_SRC_COLOR;
            case RS_DstBlend:
                return GL_DST_COLOR;
            case RS_SrcAlpha:
                return GL_SRC_ALPHA;
            case RS_DstAlpha:
                return GL_DST_COLOR;
                
            default:
                return type;
        }
    }
    
    inline GLenum render_state_param_to_gl_state_param(RenderStateParam param) {
        switch(param) {
            case RSP_TextureWrapClamp:
                return GL_CLAMP_TO_EDGE;
            case RSP_TextureWrapRepeat:
                return GL_REPEAT;
            case RSP_TextureWrapClampToBorder:
                return GL_CLAMP_TO_BORDER;
            case RSP_TextureWrapMirror:
                return GL_MIRRORED_REPEAT;
                
            case RSP_StencilOpZero:
                return GL_ZERO;
            case RSP_StencilOpKeep:
                return GL_KEEP;
            case RSP_StencilOpReplace:
                return GL_REPLACE;
            case RSP_StencilOpIncr:
                return GL_INCR;
            case RSP_StencilOpDecr:
                return GL_DECR;
            case RSP_StencilOpInvert:
                return GL_INVERT;
            case RSP_StencilOpIncrWrap:
                return GL_INCR_WRAP;
            case RSP_StencilOpDecrWrap:
                return GL_DECR_WRAP;
                
            case RSP_BlendOpAdd:
                return GL_FUNC_ADD;
            case RSP_BlendOpSubtract:
                return GL_FUNC_SUBTRACT;
            case RSP_BlendOpRevSubtract:
                return GL_FUNC_REVERSE_SUBTRACT;
            case RSP_BlendOpMax:
                return GL_MAX;
            case RSP_BlendOpMin:
                return GL_MIN;
                
            case RSP_BlendFuncZero:
                return GL_ZERO;
            case RSP_BlendFuncOne:
                return GL_ONE;
            case RSP_BlendFuncSrcColor:
                return GL_SRC_COLOR;
            case RSP_BlendFuncOneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case RSP_BlendFuncDstColor:
                return GL_DST_COLOR;
            case RSP_BlendFuncOneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;
            case RSP_BlendFuncSrcAlpha:
                return GL_SRC_ALPHA;
            case RSP_BlendFuncOneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case RSP_BlendFuncDstAlpha:
                return GL_DST_ALPHA;
            case RSP_BlendFuncOneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
            case RSP_ColorOpAdd:
                return GL_ADD;
            case RSP_ColorOpModulate:
                return GL_MODULATE;
#endif
            case RSP_CompNever:
                return GL_NEVER;
            case RSP_CompAlways:
                return GL_NEVER;
            case RSP_CompLess:
                return GL_LESS;
            case RSP_CompLEqual:
                return GL_LEQUAL;
            case RSP_CompEqual:
                return GL_EQUAL;
            case RSP_CompGreater:
                return GL_GREATER;
            case RSP_CompGEqual:
                return GL_GEQUAL;
            case RSP_CompNotEqual:
                return GL_NOTEQUAL;
                
            case RSP_Enable:
            case RSP_Disable: 
                return param;
                
            default:
                return 0;
        }
        return 0;
    }

    
    // cg generic attribute binding fpr vp40?
    // and it does not work for ATI & OSX GPU according to nvidia's document
    // ....
    enum AttributeLocations {
        AL_Position = 0,
        AL_BlendWeight,
        AL_Normal,
        AL_Diffuse,
        AL_Specular,
        AL_Tessfactor,
        AL_PSize,
        AL_BlendIndices,
        AL_TexCoord0,
        AL_TexCoord1,
        AL_TexCoord2,
        AL_TexCoord3,
        AL_TexCoord4,
        AL_TexCoord5,
        AL_TexCoord6,
        AL_TexCoord7,
        AL_Tangent,
        AL_Binormal,
        AL_NumLocations, // 18
    };
    
    
    static struct {
        const char* semantic;
        const char* attrib;
    } semantic_name_attribute_name_map[17] = {
        { "POSITION",       "ATTR0" },
        { "BLENDWEIGHT",    "ATTR1" },
        { "NORMAL",         "ATTR2" },
        { "COLOR0",         "ATTR3" },
        { "COLOR1",         "ATTR4" },
        { "PSIZE",          "ATTR6" },
        { "BLENDINDICIES",  "ATTR7" },
        { "TEXCOORD0",      "ATTR8" },
        { "TEXCOORD1",      "ATTR9" },
        { "TEXCOORD2",      "ATTR10" },
        { "TEXCOORD3",      "ATTR11" },
        { "TEXCOORD4",      "ATTR12" },
        { "TEXCOORD5",      "ATTR13" },
        { "TEXCOORD6",      "ATTR14" },
        { "TEXCOORD7",      "ATTR15" },
        { "BINORMAL",       "ATTR16" },
        { "TANGENT",        "ATTR17" }
    };
    
    inline int32 vertex_usage_to_attribute_location(VertexUsage usage) {
        switch(usage) {
        case VU_Binormal:       return AL_Binormal;
        case VU_BlendWeight:    return AL_BlendWeight;
        case VU_Diffuse:        return AL_Diffuse;
        case VU_Normal:         return AL_Normal;
        case VU_Position:       return AL_Position;
        case VU_Specular:       return AL_Specular;
        case VU_Tangent:        return AL_Tangent;
        case VU_UV:             return AL_TexCoord0;
        case VU_BlendIndices:   return AL_BlendIndices;
        case VU_PSize:          return AL_PSize;
            default:
                return -1;
        }
    }
    inline GLenum render_state_param_to_blend_equation(RenderStateParam param) {
        switch(param) {
            case RSP_BlendOpAdd: return GL_FUNC_ADD;
            case RSP_BlendOpSubtract: return GL_FUNC_SUBTRACT;
            case RSP_BlendOpRevSubtract: return GL_FUNC_REVERSE_SUBTRACT;
            case RSP_BlendOpMax: return GL_MAX;
            case RSP_BlendOpMin: return GL_MIN;
            default:
                return GL_FUNC_ADD;
        }
    }
    
    
} // namespace ukn


#endif
