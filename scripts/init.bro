##! Log writer for sending logs to ZeroMQ.

module LogZeroMQ;

export {
	## ZeroMQ hostname or IP address.  Bro will send logs to this host.
	const zmq_hostname = "" &redef;

	## ZeroMQ port number.  Bro will connect to this TCP port number.
	const zmq_port = 0 &redef;
}
