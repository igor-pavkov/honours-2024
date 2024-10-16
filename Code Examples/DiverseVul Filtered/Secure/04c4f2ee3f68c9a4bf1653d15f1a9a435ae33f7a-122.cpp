static int vmx_handle_exit(struct kvm_vcpu *vcpu, fastpath_t exit_fastpath)
{
	int ret = __vmx_handle_exit(vcpu, exit_fastpath);

	/*
	 * Even when current exit reason is handled by KVM internally, we
	 * still need to exit to user space when bus lock detected to inform
	 * that there is a bus lock in guest.
	 */
	if (to_vmx(vcpu)->exit_reason.bus_lock_detected) {
		if (ret > 0)
			vcpu->run->exit_reason = KVM_EXIT_X86_BUS_LOCK;

		vcpu->run->flags |= KVM_RUN_X86_BUS_LOCK;
		return 0;
	}
	return ret;
}