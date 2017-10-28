
#include "Plugin.h"

namespace plugin { namespace NCSA_ZeroMQWriter { Plugin plugin; } }

using namespace plugin::NCSA_ZeroMQWriter;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "NCSA::ZeroMQWriter";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;
	return config;
	}
