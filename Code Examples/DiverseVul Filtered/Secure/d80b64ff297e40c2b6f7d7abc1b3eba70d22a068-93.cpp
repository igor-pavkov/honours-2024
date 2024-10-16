static void svm_clear_current_vmcb(struct vmcb *vmcb)
{
	int i;

	for_each_online_cpu(i)
		cmpxchg(&per_cpu(svm_data, i)->current_vmcb, vmcb, NULL);
}