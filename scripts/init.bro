##! Log writer for sending logs to ZeroMQ.

module LogZeroMQ;

export {
	## ZeroMQ hostname or IP address.  Bro will send logs to this host.
	const zmq_hostname = "" &redef;

	## ZeroMQ port number.  Bro will connect to this TCP port number.
	const zmq_port = 0 &redef;

	## ZeroMQ high water mark.  Maximum number of log messages per
	## log filter that will be queued in memory if the connection to a
	## subscriber is interrupted.
	const zmq_hwm = 1000 &redef;

	## The number of milliseconds to wait for unsent messages to be sent
	## before discarding them.  This is applicable only when Bro attempts to
	## disconnect a socket, which happens only when Bro is terminating.
	const zmq_linger: int = 3000 &redef;
}
