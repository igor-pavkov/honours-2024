static long perf_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(PERF_EVENT_IOC_SET_FILTER):
	case _IOC_NR(PERF_EVENT_IOC_ID):
	case _IOC_NR(PERF_EVENT_IOC_QUERY_BPF):
	case _IOC_NR(PERF_EVENT_IOC_MODIFY_ATTRIBUTES):
		/* Fix up pointer size (usually 4 -> 8 in 32-on-64-bit case */
		if (_IOC_SIZE(cmd) == sizeof(compat_uptr_t)) {
			cmd &= ~IOCSIZE_MASK;
			cmd |= sizeof(void *) << IOCSIZE_SHIFT;
		}
		break;
	}
	return perf_ioctl(file, cmd, arg);
}