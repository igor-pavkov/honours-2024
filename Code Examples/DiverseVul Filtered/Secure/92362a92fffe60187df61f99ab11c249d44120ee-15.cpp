static int _send_slurmd_conf_lite (int fd, slurmd_conf_t *cf)
{
	int len;
	Buf buffer = init_buf(0);
	slurm_mutex_lock(&cf->config_mutex);
	pack_slurmd_conf_lite(cf, buffer);
	slurm_mutex_unlock(&cf->config_mutex);
	len = get_buf_offset(buffer);
	safe_write(fd, &len, sizeof(int));
	safe_write(fd, get_buf_data(buffer), len);
	free_buf(buffer);
	return (0);
 rwfail:
	return (-1);
}