static int __init psi_proc_init(void)
{
	if (psi_enable) {
		proc_mkdir("pressure", NULL);
		proc_create("pressure/io", 0, NULL, &psi_io_fops);
		proc_create("pressure/memory", 0, NULL, &psi_memory_fops);
		proc_create("pressure/cpu", 0, NULL, &psi_cpu_fops);
	}
	return 0;
}