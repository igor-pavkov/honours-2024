static int kvm_read_hva(void *data, void __user *hva, int len)
{
	return __copy_from_user(data, hva, len);
}