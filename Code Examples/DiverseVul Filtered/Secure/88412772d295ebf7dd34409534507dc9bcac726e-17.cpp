XMLRPC_VALUE XMLRPC_RequestSetError (XMLRPC_REQUEST request, XMLRPC_VALUE error) {
	if (request && error) {
		if (request->error) {
			XMLRPC_CleanupValue (request->error);
		}
		request->error = XMLRPC_CopyValue (error);
		return request->error;
	}
	return NULL;
}