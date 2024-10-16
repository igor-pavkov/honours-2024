int sys_get_number_of_cores(void)
{
	int ret = -1;

#if defined(HAVE_SYSCONF)
#if defined(_SC_NPROCESSORS_ONLN)
	ret = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
#if defined(_SC_NPROCESSORS_CONF)
	if (ret < 1) {
		ret = (int)sysconf(_SC_NPROCESSORS_CONF);
	}
#endif
#elif defined(HAVE_SYSCTL) && defined(CTL_HW)
	int name[2];
	unsigned int len = sizeof(ret);

	name[0] = CTL_HW;
#if defined(HW_AVAILCPU)
	name[1] = HW_AVAILCPU;

	if (sysctl(name, 2, &ret, &len, NULL, 0) == -1) {
		ret = -1;
	}
#endif
#if defined(HW_NCPU)
	if(ret < 1) {
		name[0] = CTL_HW;
		name[1] = HW_NCPU;
		if (sysctl(nm, 2, &count, &len, NULL, 0) == -1) {
			ret = -1;
		}
	}
#endif
#endif
	if (ret < 1) {
		ret = 1;
	}
	return ret;
}