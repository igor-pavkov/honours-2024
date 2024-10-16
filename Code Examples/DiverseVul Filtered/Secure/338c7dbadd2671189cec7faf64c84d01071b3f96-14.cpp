static int kvm_cpu_hotplug(struct notifier_block *notifier, unsigned long val,
			   void *v)
{
	int cpu = (long)v;

	val &= ~CPU_TASKS_FROZEN;
	switch (val) {
	case CPU_DYING:
		printk(KERN_INFO "kvm: disabling virtualization on CPU%d\n",
		       cpu);
		hardware_disable();
		break;
	case CPU_STARTING:
		printk(KERN_INFO "kvm: enabling virtualization on CPU%d\n",
		       cpu);
		hardware_enable();
		break;
	}
	return NOTIFY_OK;
}