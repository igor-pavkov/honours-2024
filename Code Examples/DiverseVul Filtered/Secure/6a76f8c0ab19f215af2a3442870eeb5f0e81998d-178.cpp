static void fpid_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&ftrace_lock);
}