static ssize_t support_poll_for_event_show(struct device_driver *dd, char *buf)
{
	return sprintf(buf, "%u\n", support_poll_for_event);
}