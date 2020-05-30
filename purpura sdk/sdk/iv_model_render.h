#pragma once

#include "materials.h"
#include "studio.h"

class i_client_renderable;
class c_studiohdr;
class i_mat_render_context;
class data_cache_handle_t;
class i_texture;
class i_material_var;
class keyvalues;
struct model_t;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct ray_t;
struct drawmodelinfo_t;
struct studiohwdata_t;
struct materiallightingstate_t;
struct colormeshinfo_t;

typedef unsigned short modelinstancehandle_t;
typedef void* lightcachehandle_t;
typedef void* studiodecalhandle_t;
typedef int overridetype_t;
typedef int imageformat;
typedef int vertexformat_t;
typedef int materialpropertytypes_t;

struct draw_model_state_t
{
    studio_hdr_t* m_pstudiohdr;
    studiohwdata_t* m_pstudiohwdata;
    i_client_renderable* m_prenderable;
    const matrix3x4_t* m_pmodeltoworld;
    studiodecalhandle_t     m_decals;
    int                     m_drawflags;
    int                     m_lod;
};

struct model_render_info_t
{
    vec3_t                  origin;
    vec3_t                  angles;
    char                    pad[4];
    i_client_renderable*    prenderable;
    const model_t*          pmodel;
    const matrix3x4_t*      pmodeltoworld;
    const matrix3x4_t*      plightingoffset;
    const vec3_t*           plightingorigin;
    int                     flags;
    int                     entity_index;
    int                     skin;
    int                     body;
    int                     hitboxset;
    modelinstancehandle_t   instance;

    model_render_info_t()
    {
        pmodeltoworld = 0;
        plightingoffset = 0;
        plightingorigin = 0;
    }
};

class iv_model_render {
public:
	void override_material(i_material* material) {
		using original_fn = void(__thiscall*)(iv_model_render*, i_material*, int, int);
		return (*(original_fn**)this)[1](this, material, 0, 0);
	}
};
