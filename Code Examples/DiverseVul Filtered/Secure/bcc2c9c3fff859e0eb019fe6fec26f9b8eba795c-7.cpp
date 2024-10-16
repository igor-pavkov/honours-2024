static void kvp_update_mem_state(int pool)
{
	FILE *filep;
	size_t records_read = 0;
	struct kvp_record *record = kvp_file_info[pool].records;
	struct kvp_record *readp;
	int num_blocks = kvp_file_info[pool].num_blocks;
	int alloc_unit = sizeof(struct kvp_record) * ENTRIES_PER_BLOCK;

	kvp_acquire_lock(pool);

	filep = fopen(kvp_file_info[pool].fname, "r");
	if (!filep) {
		kvp_release_lock(pool);
		syslog(LOG_ERR, "Failed to open file, pool: %d", pool);
		exit(-1);
	}
	while (!feof(filep)) {
		readp = &record[records_read];
		records_read += fread(readp, sizeof(struct kvp_record),
					ENTRIES_PER_BLOCK * num_blocks,
					filep);

		if (!feof(filep)) {
			/*
			 * We have more data to read.
			 */
			num_blocks++;
			record = realloc(record, alloc_unit * num_blocks);

			if (record == NULL) {
				syslog(LOG_ERR, "malloc failed");
				exit(-1);
			}
			continue;
		}
		break;
	}

	kvp_file_info[pool].num_blocks = num_blocks;
	kvp_file_info[pool].records = record;
	kvp_file_info[pool].num_records = records_read;

	kvp_release_lock(pool);
}