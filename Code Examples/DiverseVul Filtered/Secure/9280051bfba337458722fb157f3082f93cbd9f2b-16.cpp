static int shutdown_other_smbds(struct db_record *rec,
				const struct connections_key *key,
				const struct connections_data *crec,
				void *private_data)
{
	const char *ip = (const char *)private_data;

	if (!process_exists(crec->pid)) {
		return 0;
	}

	if (procid_is_me(&crec->pid)) {
		return 0;
	}

	if (strcmp(ip, crec->addr) != 0) {
		return 0;
	}

	DEBUG(0,("shutdown_other_smbds: shutting down pid %u "
		 "(IP %s)\n", (unsigned int)procid_to_pid(&crec->pid), ip));

	messaging_send(smbd_messaging_context(), crec->pid, MSG_SHUTDOWN,
		       &data_blob_null);
	return 0;
}