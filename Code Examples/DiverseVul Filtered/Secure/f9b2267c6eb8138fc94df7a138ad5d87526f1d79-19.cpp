static void ldapsrv_notification_retry_done(struct tevent_req *subreq)
{
	struct ldapsrv_service *service =
		tevent_req_callback_data(subreq,
		struct ldapsrv_service);
	struct ldapsrv_connection *conn = NULL;
	struct ldapsrv_connection *conn_next = NULL;
	bool ok;

	service->notification.retry = NULL;

	ok = tevent_wakeup_recv(subreq);
	TALLOC_FREE(subreq);
	if (!ok) {
		/* ignore */
	}

	for (conn = service->connections; conn != NULL; conn = conn_next) {
		struct ldapsrv_call *call = conn->pending_calls;

		conn_next = conn->next;

		if (conn->pending_calls == NULL) {
			continue;
		}

		if (conn->active_call != NULL) {
			continue;
		}

		DLIST_DEMOTE(conn->pending_calls, call);
		call->notification.generation =
				service->notification.generation;

		/* queue the call in the global queue */
		subreq = ldapsrv_process_call_send(call,
						   conn->connection->event.ctx,
						   conn->service->call_queue,
						   call);
		if (subreq == NULL) {
			ldapsrv_terminate_connection(conn,
					"ldapsrv_process_call_send failed");
			continue;
		}
		tevent_req_set_callback(subreq, ldapsrv_call_process_done, call);
		conn->active_call = subreq;
	}

	ldapsrv_notification_retry_setup(service, false);
}