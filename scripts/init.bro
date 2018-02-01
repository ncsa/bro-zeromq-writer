##! Log writer for sending logs to ZeroMQ.

module LogZeroMQ;

export {
	## ZeroMQ endpoint of the subscriber.  This must include both a transport
	## protocol (can be "tcp" or "ipc") and a transport-specific address.
	##
	## For "tcp" transport, the address is a hostname or IP address followed
	## by a colon, followed by a TCP port number (e.g. "tcp://localhost:1234").
	## For "ipc" transport, the address is the pathname of a UNIX domain
	## socket (e.g. "ipc:///usr/local/subscriber/socket").
	##
	## This value can be overridden on a per-filter basis in a
	## filter's "config" table.
	const endpoint = "" &redef;

	## ZeroMQ high water mark.  Maximum number of log messages per
	## log filter that will be queued in memory if the connection to a
	## subscriber is interrupted.
	const zmq_hwm = 1000 &redef;

	## The maximum number of milliseconds to wait for unsent messages to be
	## sent before discarding them.  This is applicable only when this plugin
	## attempts to disconnect a socket (which happens only when Bro is
	## terminating).  A value of -1 means Bro will not terminate until all
	## unsent messages have been sent.
	const zmq_linger: int = 3000 &redef;
}
