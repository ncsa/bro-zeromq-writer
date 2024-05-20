#pragma once

#include <zeek/plugin/Plugin.h>

namespace zeek::plugin {
namespace NCSA_ZeroMQWriter {

class Plugin : public zeek::plugin::Plugin
{
protected:
	// Overridden from plugin::Plugin.
	zeek::plugin::Configuration Configure() override;
};

extern Plugin plugin;

}
}
