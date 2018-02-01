##! Enable log output to ZeroMQ.

module LogZeroMQ;

export {
    ## Optionally ignore any specified :bro:type:`Log::ID` from being sent to
    ## ZeroMQ.
    const excluded_log_ids: set[Log::ID] &redef;

    ## If you want to explicitly only send certain :bro:type:`Log::ID`
    ## streams, add them to this set.  If the set remains empty, all will
    ## be sent.  The :bro:id:`LogZeroMQ::excluded_log_ids` option
    ## will remain in effect as well.
    const send_logs: set[Log::ID] &redef;
}

event bro_init() &priority=-5
    {
    if ( endpoint == "" )
        return;

    for ( stream_id in Log::active_streams )
        {
        if ( stream_id in excluded_log_ids ||
             (|send_logs| > 0 && stream_id !in send_logs) )
            next;

        local filter: Log::Filter = [$name = "default-zmq",
                                     $writer = Log::WRITER_ZEROMQ,
                                     $interv = 0 sec];
        Log::add_filter(stream_id, filter);
        }
    }
