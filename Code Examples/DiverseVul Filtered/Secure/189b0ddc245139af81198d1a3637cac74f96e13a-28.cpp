static int do_proc_dopipe_max_size_conv(unsigned long *lvalp,
					unsigned int *valp,
					int write, void *data)
{
	if (write) {
		unsigned int val;

		val = round_pipe_size(*lvalp);
		if (val == 0)
			return -EINVAL;

		*valp = val;
	} else {
		unsigned int val = *valp;
		*lvalp = (unsigned long) val;
	}

	return 0;
}