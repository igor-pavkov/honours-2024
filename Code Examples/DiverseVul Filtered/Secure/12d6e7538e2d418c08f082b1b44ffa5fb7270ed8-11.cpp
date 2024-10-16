static int kvm_read_hva_atomic(void *data, void __user *hva, int len)
{
	return __copy_from_user_inatomic(data, hva, len);
}