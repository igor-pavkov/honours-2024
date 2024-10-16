int netdev_printk(const char *level, const struct net_device *dev,
		  const char *format, ...)
{
	struct va_format vaf;
	va_list args;
	int r;

	va_start(args, format);

	vaf.fmt = format;
	vaf.va = &args;

	r = __netdev_printk(level, dev, &vaf);
	va_end(args);

	return r;
}