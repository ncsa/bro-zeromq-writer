#include "Plugin.h"
#include "ZeroMQ.h"

namespace plugin { namespace NCSA_ZeroMQWriter { Plugin plugin; } }

using namespace plugin::NCSA_ZeroMQWriter;

plugin::Configuration Plugin::Configure()
	{
	AddComponent(new ::logging::Component("ZeroMQ", ::logging::writer::ZeroMQ::Instantiate));

	plugin::Configuration config;
	config.name = "NCSA::ZeroMQWriter";
	config.description = "ZeroMQ log writer";
	config.version.major = 0;
	config.version.minor = 2;
	return config;
	}
