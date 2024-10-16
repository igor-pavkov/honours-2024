static int ldapsrv_call_destructor(struct ldapsrv_call *call)
{
	if (call->conn == NULL) {
		return 0;
	}

	DLIST_REMOVE(call->conn->pending_calls, call);

	call->conn = NULL;
	return 0;
}