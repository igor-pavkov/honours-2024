static void control_ops_disable_all(struct ftrace_ops *ops)
{
	int cpu;

	for_each_possible_cpu(cpu)
		*per_cpu_ptr(ops->disabled, cpu) = 1;
}