#pragma once

class i_net_channel;
class bf_read;
class bf_write;

class i_net_message
{
public:
	virtual ~i_net_message() {};
	virtual void set_net_channel(i_net_channel*) = 0;
	virtual void set_reliable(bool) = 0;
	virtual bool process(void) = 0;
	virtual bool read_from_buffer(bf_read&) = 0;
	virtual bool write_from_buffer(bf_write&) = 0;
	virtual bool is_reliable(void) = 0;
	virtual int get_type(void) const = 0;
	virtual int get_group(void) const = 0;
	virtual const char* get_name(void) const = 0;
	virtual i_net_channel* get_net_channel(void) const = 0;
	virtual const char* to_string(void) const = 0;
};