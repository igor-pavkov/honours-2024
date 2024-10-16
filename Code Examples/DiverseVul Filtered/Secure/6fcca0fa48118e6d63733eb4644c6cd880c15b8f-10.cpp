static ssize_t psi_cpu_write(struct file *file, const char __user *user_buf,
			     size_t nbytes, loff_t *ppos)
{
	return psi_write(file, user_buf, nbytes, PSI_CPU);
}