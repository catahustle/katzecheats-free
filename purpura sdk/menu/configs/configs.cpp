#include "../settings.h"
#include "configs.h"

void c_config_sys::setup()
{
	
}

void c_config_sys::setup_value(int& value, int def, std::string name)
{
	value = def;
	ints.push_back(new config_value< int >(name, &value));
}

void c_config_sys::setup_value(float& value, float def, std::string name)
{
	value = def;
	floats.push_back(new config_value< float >(name, &value));
}

void c_config_sys::setup_value(bool& value, bool def, std::string name)
{
	value = def;
	bools.push_back(new config_value< bool >(name, &value));
}

void c_config_sys::setup_value(color& value, float def, std::string name)
{
	value.r = def;
	value.g = def;
	value.b = def;
	value.a = def;

	floats.push_back(new config_value< float >(name + "_red", &value.r));
	floats.push_back(new config_value< float >(name + "_green", &value.g));
	floats.push_back(new config_value< float >(name + "_blue", &value.b));
	floats.push_back(new config_value< float >(name + "_alpha", &value.a));
}

static char name[] = "impact csgo";

void c_config_sys::save(std::string config_name)
{
	static wchar_t path[MAX_PATH];
	char path_2_string[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
		sprintf(path_2_string, "%ls", path);
		folder = std::string(path_2_string) + "\\impact_csgo\\";
		file = std::string(path_2_string) + "\\impact_csgo\\" + config_name + ".cool";
	}

	CreateDirectoryA(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileStringA(name, value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileStringA(name, value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileStringA(name, value->name.c_str(), *value->value ? "yes" : "no", file.c_str());
}

void c_config_sys::load(std::string config_name)
{
	static wchar_t path[MAX_PATH];
	char path_2_string[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
		sprintf(path_2_string, "%ls", path);
		folder = std::string(path_2_string) + "\\impact_csgo\\";
		file = std::string(path_2_string) + "\\impact_csgo\\" + config_name + ".cool";
	}

	CreateDirectoryA(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileStringA(name, value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileStringA(name, value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileStringA(name, value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, "yes");
	}
}

c_config_sys* config_system = new c_config_sys();