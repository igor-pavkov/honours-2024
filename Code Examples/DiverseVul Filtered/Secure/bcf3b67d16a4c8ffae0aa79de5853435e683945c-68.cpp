static ssize_t dbg_lvl_show(struct device_driver *dd, char *buf)
{
	return sprintf(buf, "%u\n", megasas_dbg_lvl);
}