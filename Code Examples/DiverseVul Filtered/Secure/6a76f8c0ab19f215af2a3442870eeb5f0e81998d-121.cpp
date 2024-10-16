static int ftrace_profile_init(void)
{
	int cpu;
	int ret = 0;

	for_each_online_cpu(cpu) {
		ret = ftrace_profile_init_cpu(cpu);
		if (ret)
			break;
	}

	return ret;
}