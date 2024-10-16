const struct perf_event *perf_get_event(struct file *file)
{
	if (file->f_op != &perf_fops)
		return ERR_PTR(-EINVAL);

	return file->private_data;
}