void arch_setup_new_exec(void)
{
	if (radix_enabled())
		return;
	hash__setup_new_exec();
}