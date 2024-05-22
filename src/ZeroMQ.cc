// See the file "COPYING" for copyright.
//
// Log writer for writing to ZeroMQ

#include <cstring>
#include <cstdlib>
#include <zmq.h>
#include <zeek/Desc.h>
#include <unistd.h>
#include "ZeroMQ.h"
#include "zeromqwriter.bif.h"

using namespace zeek::logging;
using namespace writer;

// Constructor is called once for each log filter that uses this log writer.
ZeroMQ::ZeroMQ(WriterFrontend* frontend): WriterBackend(frontend), formatter(nullptr), zmq_context(nullptr), zmq_publisher(nullptr)
{
    // Get default endpoint of subscriber.
    endpoint.assign(
        (const char*) BifConst::LogZeroMQ::endpoint->Bytes(),
        BifConst::LogZeroMQ::endpoint->Len());

    // Get user-specified value for ZeroMQ high water mark.
    zmq_hwm = BifConst::LogZeroMQ::zmq_hwm;

    // Get user-specified value for ZeroMQ linger time.
    zmq_linger = BifConst::LogZeroMQ::zmq_linger;

    // Get user-specified value for ZeroMQ connect pause
    zmq_connect_pause = BifConst::LogZeroMQ::zmq_connect_pause;

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

    // Determine endpoint of subscriber.  Values from the Bro
    // script "config" table (in Log::Filter) override value of the Bro script
    // constant "endpoint".
    string use_endpoint = endpoint;
    string cfg_endpoint = GetConfigValue(info, "endpoint");

    if (!cfg_endpoint.empty())
        use_endpoint = cfg_endpoint;

    // Create zmq socket
    zmq_publisher = zmq_socket(zmq_context, ZMQ_PUB);
    if (!zmq_publisher) {
        Error(Fmt("Failed to create zmq socket for log path '%s': %s", log_path, strerror(errno)));
        return false;
    }

    // Set the LINGER time.
    if (zmq_setsockopt(zmq_publisher, ZMQ_LINGER, &zmq_linger, sizeof(int))) {
        // This is not a critical failure, so we don't return false here.
        Warning(Fmt("Failed to set ZMQ_LINGER for log path '%s': %s", log_path, strerror(errno)));
    }

    // Set the ZeroMQ high water mark.
    if (zmq_setsockopt(zmq_publisher, ZMQ_SNDHWM, &zmq_hwm, sizeof(int))) {
        // This is not a critical failure, so we don't return false here.
        Warning(Fmt("Failed to set ZMQ_SNDHWM for log path '%s': %s", log_path, strerror(errno)));
    }

    // Connect to the zmq subscriber
    if (zmq_connect(zmq_publisher, Fmt("%s", use_endpoint.c_str()))) {
        Error(Fmt("Failed to connect zmq socket for log path '%s': %s", log_path, strerror(errno)));

        // We must close the socket here, because DoFinish will not be called.
        zmq_close(zmq_publisher);

        return false;
    }

    // Initialize the formatter
    formatter = new threading::formatter::JSON(this, threading::formatter::JSON::TS_EPOCH);

    usleep(zmq_connect_pause * 1000);
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
