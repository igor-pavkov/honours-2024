static void save_msrs(struct kvm_msr_entry *e, int n)
{
	int i;

	for (i = 0; i < n; ++i)
		rdmsrl(e[i].index, e[i].data);
}