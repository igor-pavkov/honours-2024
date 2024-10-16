static void kvp_update_file(int pool)
{
	FILE *filep;
	size_t bytes_written;

	/*
	 * We are going to write our in-memory registry out to
	 * disk; acquire the lock first.
	 */
	kvp_acquire_lock(pool);

	filep = fopen(kvp_file_info[pool].fname, "w");
	if (!filep) {
		kvp_release_lock(pool);
		syslog(LOG_ERR, "Failed to open file, pool: %d", pool);
		exit(-1);
	}

	bytes_written = fwrite(kvp_file_info[pool].records,
				sizeof(struct kvp_record),
				kvp_file_info[pool].num_records, filep);

	fflush(filep);
	kvp_release_lock(pool);
}