static int sysctl_check_table_array(const char *path, struct ctl_table *table)
{
	int err = 0;

	if ((table->proc_handler == proc_douintvec) ||
	    (table->proc_handler == proc_douintvec_minmax)) {
		if (table->maxlen != sizeof(unsigned int))
			err |= sysctl_err(path, table, "array not allowed");
	}

	return err;
}