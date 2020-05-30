#pragma once

#include "i_app_system.h"

class i_localize_text_query
{
public:
	virtual int compute_text_width(const wchar_t* pString) = 0;
};

class i_localization_check_callback
{
public:
	virtual void on_change_callback() = 0;
};

class i_localize : public i_app_system
{
public:
	virtual bool					add_file(const char*, const char*, bool) = 0;
	virtual void					remove_all() = 0;
	virtual wchar_t*				find(const char*) = 0;
	virtual const wchar_t*			find_safe(const char* token_name) = 0;
	virtual int						convert_ansi_to_unicode(const char* ansi, wchar_t* unicode, int unicode_buffer_size_in_bytes) = 0;
	virtual int						convert_unicode_to_ansi(const wchar_t* unicode, char* ansi, int ansi_buffer_size) = 0;
	virtual unsigned				find_index(const char* token_name) = 0;
	virtual void					construct_string(wchar_t* unicode_output, int unicode_buffer_size_in_bytes, const wchar_t* format_string, int num_format_parameters, ...) = 0;
	virtual const char*				get_name_by_index(unsigned index) = 0;
	virtual wchar_t*				get_value_by_index(unsigned index) = 0;
};