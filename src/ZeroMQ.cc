// See the file "COPYING" for copyright.
//
// Log writer for writing to ZeroMQ

#include <cstring>
#include <cstdlib>
#include <zmq.h>

#include "ZeroMQ.h"
#include "zeromqwriter.bif.h"

using namespace logging;
using namespace writer;

// Constructor is called once for each log filter that uses this log writer.
ZeroMQ::ZeroMQ(WriterFrontend* frontend): WriterBackend(frontend), formatter(nullptr), zmq_context(nullptr), zmq_publisher(nullptr)
{
    // Get default host and port of subscriber from the Bro script constants.
    zmq_hostname.assign(
        (const char*) BifConst::LogZeroMQ::zmq_hostname->Bytes(),
        BifConst::LogZeroMQ::zmq_hostname->Len());
    zmq_port = BifConst::LogZeroMQ::zmq_port;

    // Create zmq context
    zmq_context = zmq_ctx_new();
}

ZeroMQ::~ZeroMQ()
{
    zmq_ctx_destroy(zmq_context);
}

// Do a table lookup in the "config" table (in Log::Filter) using the given
// index name.  Result is always a string, where an empty string indicates
// the given index does not exist.
string ZeroMQ::GetConfigValue(const WriterInfo& info, const string name) const
{
    map<const char*, const char*>::const_iterator it = info.config.find(name.c_str());
    if (it == info.config.end())
        return string();
    else
        return it->second;
}

// DoInit is called once for each call to the constructor, but in a different
// thread.
bool ZeroMQ::DoInit(const WriterInfo& info, int num_fields, const threading::Field* const* fields)
{
    // The Bro log path name.  This would normally be something like "conn",
    // but if there are multiple log filters with same log path, then Bro
    // renames the duplicates like "conn-2", "conn-3", etc.
    log_path = info.path;

    // Determine host and port of subscriber.  Values from the Bro script
    // "config" table (in Log::Filter) override values from the Bro script
    // constants "zmq_hostname" and "zmq_port".
    string hostname = zmq_hostname;
    int port = zmq_port;

    string cfg_hostname = GetConfigValue(info, "hostname");
    string cfg_port = GetConfigValue(info, "port");

    if (!cfg_hostname.empty())
        hostname = cfg_hostname;

    if (!cfg_port.empty())
        port = strtoul(cfg_port.c_str(), nullptr, 10);

    // Create zmq socket
    zmq_publisher = zmq_socket(zmq_context, ZMQ_PUB);
    if (!zmq_publisher) {
        Error(Fmt("Failed to create zmq socket for log path '%s': %s", log_path, strerror(errno)));
        return false;
    }

    // Set the LINGER time to prevent "broctl stop" from hanging when there
    // are unsent log messages and a connection to a subscriber is interrupted.
    int millisecs = 0;
    if (zmq_setsockopt(zmq_publisher, ZMQ_LINGER, &millisecs, sizeof(int))) {
        // This is not a critical failure, so we don't return false here.
        Warning(Fmt("Failed to set ZMQ_LINGER for log path '%s': %s", log_path, strerror(errno)));
    }

    // Connect to the zmq subscriber
    if (zmq_connect(zmq_publisher, Fmt("tcp://%s:%d", hostname.c_str(), port))) {
        Error(Fmt("Failed to connect zmq socket for log path '%s': %s", log_path, strerror(errno)));

        // We must close the socket here, because DoFinish will not be called.
        zmq_close(zmq_publisher);

        return false;
    }

    // Initialize the formatter
    formatter = new threading::formatter::JSON(this, threading::formatter::JSON::TS_EPOCH);

    return true;
}

// Free resources acquired in DoInit.
bool ZeroMQ::DoFinish(double network_time)
{
    zmq_close(zmq_publisher);
    delete formatter;

    return true;
}

bool ZeroMQ::DoWrite(int num_fields, const threading::Field* const* fields, threading::Value** vals)
{
    ODesc buffer;
    buffer.Clear();

    formatter->Describe(&buffer, num_fields, fields, vals);

    const char* msg = (const char*)buffer.Bytes();

    // Send a ZeroMQ multi-part message, where 1st part is just the Bro log
    // path, and 2nd part is the log record in JSON format.
    int nbytes = zmq_send(zmq_publisher, log_path, strlen(log_path), ZMQ_SNDMORE);
    if (nbytes == -1) {
        Warning(Fmt("Failed to send log path '%s' to ZeroMQ: %s", log_path, strerror(errno)));
        // Return true here just in case the next message can be sent.
        // Don't try to send 2nd part (to ensure all messages have 2 parts).
        return true;
    }

    nbytes = zmq_send(zmq_publisher, msg, strlen(msg), 0);
    if (nbytes == -1) {
        Warning(Fmt("Failed to send '%s' log to ZeroMQ: %s", log_path, strerror(errno)));
    }

    return true;
}

bool ZeroMQ::DoSetBuf(bool enabled)
{
    return true;
}

bool ZeroMQ::DoFlush(double network_time)
{
    return true;
}

bool ZeroMQ::DoRotate(const char* rotated_path, double open, double close, bool terminating)
{
    // No log rotation needed
    return FinishedRotation();
}

bool ZeroMQ::DoHeartbeat(double network_time, double current_time)
{
    return true;
}
