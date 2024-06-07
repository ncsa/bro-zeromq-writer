#include "Plugin.h"
#include "ZeroMQ.h"

namespace zeek::plugin::NCSA_ZeroMQWriter { Plugin plugin; }

using namespace zeek::plugin::NCSA_ZeroMQWriter;

zeek::plugin::Configuration Plugin::Configure()
	{
	AddComponent(new zeek::logging::Component("ZeroMQ", zeek::logging::writer::ZeroMQ::Instantiate));

	zeek::plugin::Configuration config;
	config.name = "NCSA::ZeroMQWriter";
	config.description = "ZeroMQ log writer";
	config.version.major = 0;
	config.version.minor = 3;
	return config;
	}
