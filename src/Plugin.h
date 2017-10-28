
#ifndef BRO_PLUGIN_NCSA_ZEROMQWRITER
#define BRO_PLUGIN_NCSA_ZEROMQWRITER

#include <plugin/Plugin.h>

namespace plugin {
namespace NCSA_ZeroMQWriter {

class Plugin : public ::plugin::Plugin
{
protected:
	// Overridden from plugin::Plugin.
	plugin::Configuration Configure() override;
};

extern Plugin plugin;

}
}

#endif
