#pragma once

#include "client_class.h"
#include "i_base_handle.h"
#include "studio.h"

class i_base_client_dll {
public:
	c_client_class* get_client_classes() {
		using original_fn = c_client_class * (__thiscall*)(i_base_client_dll*);
		return (*(original_fn**)this)[8](this);
	}

	bool dispatch_user_msg(int msg_type)
	{
		using original_fn = bool(__thiscall*)(void*, int, int, int, void*);
		return (*(original_fn**)this)[38](this, msg_type, 0, 0, nullptr);
	}
};

class i_client_unknown : public i_handle_entity
{
public:
	virtual void* GetCollideable() = 0;
	virtual i_client_networkable* GetClientNetworkable() = 0;
	virtual void* GetClientRenderable() = 0;
	virtual void* GetIClientEntity() = 0;
	virtual void* GetBaseEntity() = 0;
	virtual void* GetClientThinkable() = 0;
	//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual void* GetClientAlphaProperty() = 0;
};

typedef unsigned short ClientShadowHandle_t;
typedef unsigned short ClientRenderHandle_t;
typedef unsigned short ModelInstanceHandle_t;
class bf_read;

class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;

class i_client_thinkable
{
public:
	virtual i_client_unknown*   GetIClientUnknown() = 0;
	virtual void                ClientThink() = 0;
	virtual ClientThinkHandle_t GetThinkHandle() = 0;
	virtual void                SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void                Release() = 0;
};

class i_client_networkable
{
public:
	virtual i_client_unknown* GetIClientUnknown() = 0;
	virtual void             Release() = 0;
	virtual c_client_class* GetClientClass() = 0;
	virtual void             NotifyShouldTransmit(int state) = 0;
	virtual void             OnPreDataChanged(int updateType) = 0;
	virtual void             OnDataChanged(int updateType) = 0;
	virtual void             PreDataUpdate(int updateType) = 0;
	virtual void             PostDataUpdate(int updateType) = 0;
	virtual void             __unkn(void) = 0;
	virtual bool             IsDormant(void) = 0;
	virtual int              EntIndex(void) const = 0;
	virtual void             ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};

class i_client_renderable
{
public:
	virtual i_client_unknown* GetIClientUnknown() = 0;
	virtual vec3_t const& GetRenderOrigin(void) = 0;
	virtual vec3_t const& GetRenderAngles(void) = 0;
	virtual bool                       ShouldDraw(void) = 0;
	virtual int                        GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused(void) const {}
	virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/& instance) = 0;
	virtual int                        GetBody() = 0;
	virtual void                       GetColorModulation(float* color) = 0;
	virtual bool                       LODTest() = 0;
	virtual bool                       SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void                       SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void                       DoAnimationEvents(void) = 0;
	virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
	virtual void                       GetRenderBounds(vec3_t& mins, vec3_t& maxs) = 0;
	virtual void                       GetRenderBoundsWorldspace(vec3_t& mins, vec3_t& maxs) = 0;
	virtual void                       GetShadowRenderBounds(vec3_t& mins, vec3_t& maxs, int /*ShadowType_t*/ shadowType) = 0;
	virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool                       GetShadowCastDistance(float* pDist, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       GetShadowCastDirection(vec3_t* pDirection, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       IsShadowDirty() = 0;
	virtual void                       MarkShadowDirty(bool bDirty) = 0;
	virtual i_client_renderable* GetShadowParent() = 0;
	virtual i_client_renderable* FirstShadowChild() = 0;
	virtual i_client_renderable* NextShadowPeer() = 0;
	virtual int /*ShadowType_t*/       ShadowCastType() = 0;
	virtual void                       CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t      GetModelInstance() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int                        LookupAttachment(const char* pAttachmentName) = 0;
	virtual   bool                     GetAttachment(int number, vec3_t& origin, vec3_t& angles) = 0;
	virtual bool                       GetAttachment(int number, matrix3x4_t& matrix) = 0;
	virtual float* GetRenderClipPlane(void) = 0;
	virtual int                        GetSkin() = 0;
	virtual void                       OnThreadedDrawSetup() = 0;
	virtual bool                       UsesFlexDelayedWeights() = 0;
	virtual void                       RecordToolMessage() = 0;
	virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8_t                      OverrideAlphaModulation(uint8_t nAlpha) = 0;
	virtual uint8_t                      OverrideShadowAlphaModulation(uint8_t nAlpha) = 0;
};

class i_client_entity : public i_client_unknown, public i_client_renderable, public i_client_networkable, public i_client_thinkable {
public:
	virtual void Release(void) = 0;

	int entity_index() {
		void *nigga_toilet = (void*)(this + 0x8);

		typedef int(__thiscall *o_fn)(void*);
		return global_utils::legacy_v_function<o_fn>(nigga_toilet, 10)(nigga_toilet);
	}
};