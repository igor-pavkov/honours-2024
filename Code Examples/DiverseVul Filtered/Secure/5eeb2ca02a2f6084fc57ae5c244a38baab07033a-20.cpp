static int binder_vm_fault(struct vm_fault *vmf)
{
	return VM_FAULT_SIGBUS;
}