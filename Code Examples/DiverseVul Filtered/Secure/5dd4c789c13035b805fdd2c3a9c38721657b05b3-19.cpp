static void notify_fsp(files_struct *fsp, struct timespec when,
		       uint32_t action, const char *name)
{
	struct notify_change_event *change, *changes;
	char *tmp;

	if (fsp->notify == NULL) {
		/*
		 * Nobody is waiting, don't queue
		 */
		return;
	}

	/*
	 * Someone has triggered a notify previously, queue the change for
	 * later.
	 */

	if ((fsp->notify->num_changes > 1000) || (name == NULL)) {
		/*
		 * The real number depends on the client buf, just provide a
		 * guard against a DoS here.  If name == NULL the CN backend is
		 * alerting us to a problem.  Possibly dropped events.  Clear
		 * queued changes and send the catch-all response to the client
		 * if a request is pending.
		 */
		TALLOC_FREE(fsp->notify->changes);
		fsp->notify->num_changes = -1;
		if (fsp->notify->requests != NULL) {
			change_notify_reply(fsp->notify->requests->req,
					    NT_STATUS_OK,
					    fsp->notify->requests->max_param,
					    fsp->notify,
					    fsp->notify->requests->reply_fn);
			change_notify_remove_request(fsp->conn->sconn,
						     fsp->notify->requests);
		}
		return;
	}

	/* If we've exceeded the server side queue or received a NULL name
	 * from the underlying CN implementation, don't queue up any more
	 * requests until we can send a catch-all response to the client */
	if (fsp->notify->num_changes == -1) {
		return;
	}

	if (!(changes = talloc_realloc(
		      fsp->notify, fsp->notify->changes,
		      struct notify_change_event,
		      fsp->notify->num_changes+1))) {
		DEBUG(0, ("talloc_realloc failed\n"));
		return;
	}

	fsp->notify->changes = changes;

	change = &(fsp->notify->changes[fsp->notify->num_changes]);

	if (!(tmp = talloc_strdup(changes, name))) {
		DEBUG(0, ("talloc_strdup failed\n"));
		return;
	}

	string_replace(tmp, '/', '\\');
	change->name = tmp;	

	change->when = when;
	change->action = action;
	fsp->notify->num_changes += 1;

	if (fsp->notify->requests == NULL) {
		/*
		 * Nobody is waiting, so don't send anything. The ot
		 */
		return;
	}

	if (action == NOTIFY_ACTION_OLD_NAME) {
		/*
		 * We have to send the two rename events in one reply. So hold
		 * the first part back.
		 */
		return;
	}

	/*
	 * Someone is waiting for the change, trigger the reply immediately.
	 *
	 * TODO: do we have to walk the lists of requests pending?
	 */

	change_notify_reply(fsp->notify->requests->req,
			    NT_STATUS_OK,
			    fsp->notify->requests->max_param,
			    fsp->notify,
			    fsp->notify->requests->reply_fn);

	change_notify_remove_request(fsp->conn->sconn, fsp->notify->requests);
}