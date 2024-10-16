static int vcpu_stat_get(void *_offset, u64 *val)
{
	unsigned offset = (long)_offset;
	struct kvm *kvm;
	struct kvm_vcpu *vcpu;
	int i;

	*val = 0;
	spin_lock(&kvm_lock);
	list_for_each_entry(kvm, &vm_list, vm_list)
		kvm_for_each_vcpu(i, vcpu, kvm)
			*val += *(u32 *)((void *)vcpu + offset);

	spin_unlock(&kvm_lock);
	return 0;
}