void ldapsrv_notification_retry_setup(struct ldapsrv_service *service, bool force)
{
	struct ldapsrv_connection *conn = NULL;
	struct timeval retry;
	size_t num_pending = 0;
	size_t num_active = 0;

	if (force) {
		TALLOC_FREE(service->notification.retry);
		service->notification.generation += 1;
	}

	if (service->notification.retry != NULL) {
		return;
	}

	for (conn = service->connections; conn != NULL; conn = conn->next) {
		if (conn->pending_calls == NULL) {
			continue;
		}

		num_pending += 1;

		if (conn->pending_calls->notification.generation !=
		    service->notification.generation)
		{
			num_active += 1;
		}
	}

	if (num_pending == 0) {
		return;
	}

	if (num_active != 0) {
		retry = timeval_current_ofs(0, 100);
	} else {
		retry = timeval_current_ofs(5, 0);
	}

	service->notification.retry = tevent_wakeup_send(service,
							 service->task->event_ctx,
							 retry);
	if (service->notification.retry == NULL) {
		/* retry later */
		return;
	}

	tevent_req_set_callback(service->notification.retry,
				ldapsrv_notification_retry_done,
				service);
}