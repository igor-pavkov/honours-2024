static void deferred_cad(struct work_struct *dummy)
{
	kernel_restart(NULL);
}