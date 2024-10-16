XMLRPC_VALUE XMLRPC_RequestGetError (XMLRPC_REQUEST request) {
	return request ? request->error : NULL;
}