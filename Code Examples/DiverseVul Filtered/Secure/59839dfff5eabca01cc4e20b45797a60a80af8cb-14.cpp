static void vcpu_kick_intr(void *info)
{
#ifdef DEBUG
	struct kvm_vcpu *vcpu = (struct kvm_vcpu *)info;
	printk(KERN_DEBUG "vcpu_kick_intr %p \n", vcpu);
#endif
}