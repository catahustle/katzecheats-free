#pragma once

#include "studio.h"

class iv_model_info {
public:
	virtual					~iv_model_info() {};
	virtual const model_t*	get_model(int model_index) const = 0;
	virtual int				get_model_index(const char* name) const = 0;
	virtual const char*		get_model_name(const model_t* model) const = 0;
	studio_hdr_t*			get_studio_model(const model_t* model) {
		using original_fn = studio_hdr_t * (__thiscall*)(iv_model_info*, const model_t*);
		return (*(original_fn**)this)[32](this, model);
	}
};