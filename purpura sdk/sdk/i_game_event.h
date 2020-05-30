#pragma once

class i_game_event
{
public:
	virtual ~i_game_event() {};
	virtual const char* get_name() const = 0;	// get event name

	virtual bool  is_reliable() const = 0; // if event handled reliable
	virtual bool  is_local() const = 0; // if event is never networked
	virtual bool  is_empty(const char* keyName = 0) = 0; // check if data field exists

	// Data access
	virtual bool  get_bool(const char* keyName = 0, bool defaultValue = false) = 0;
	virtual int   get_int(const char* keyName = 0, int defaultValue = 0) = 0;
	virtual float get_float(const char* keyName = 0, float defaultValue = 0.0f) = 0;
	virtual const char* get_string(const char* keyName = 0, const char* defaultValue = "") = 0;

	virtual void set_bool(const char* keyName, bool value) = 0;
	virtual void set_int(const char* keyName, int value) = 0;
	virtual void set_float(const char* keyName, float value) = 0;
	virtual void set_string(const char* keyName, const char* value) = 0;
};