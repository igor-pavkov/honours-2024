static struct parsed_partitions *check_partition(struct gendisk *hd)
{
	struct parsed_partitions *state;
	int i, res, err;

	state = allocate_partitions(hd);
	if (!state)
		return NULL;
	state->pp_buf = (char *)__get_free_page(GFP_KERNEL);
	if (!state->pp_buf) {
		free_partitions(state);
		return NULL;
	}
	state->pp_buf[0] = '\0';

	state->disk = hd;
	snprintf(state->name, BDEVNAME_SIZE, "%s", hd->disk_name);
	snprintf(state->pp_buf, PAGE_SIZE, " %s:", state->name);
	if (isdigit(state->name[strlen(state->name)-1]))
		sprintf(state->name, "p");

	i = res = err = 0;
	while (!res && check_part[i]) {
		memset(state->parts, 0, state->limit * sizeof(state->parts[0]));
		res = check_part[i++](state);
		if (res < 0) {
			/*
			 * We have hit an I/O error which we don't report now.
			 * But record it, and let the others do their job.
			 */
			err = res;
			res = 0;
		}

	}
	if (res > 0) {
		printk(KERN_INFO "%s", state->pp_buf);

		free_page((unsigned long)state->pp_buf);
		return state;
	}
	if (state->access_beyond_eod)
		err = -ENOSPC;
	/*
	 * The partition is unrecognized. So report I/O errors if there were any
	 */
	if (err)
		res = err;
	if (res) {
		strlcat(state->pp_buf,
			" unable to read partition table\n", PAGE_SIZE);
		printk(KERN_INFO "%s", state->pp_buf);
	}

	free_page((unsigned long)state->pp_buf);
	free_partitions(state);
	return ERR_PTR(res);
}