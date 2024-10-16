int ccid_getsockopt_builtin_ccids(struct sock *sk, int len,
				  char __user *optval, int __user *optlen)
{
	u8 *ccid_array, array_len;
	int err = 0;

	if (len < ARRAY_SIZE(ccids))
		return -EINVAL;

	if (ccid_get_builtin_ccids(&ccid_array, &array_len))
		return -ENOBUFS;

	if (put_user(array_len, optlen) ||
	    copy_to_user(optval, ccid_array, array_len))
		err = -EFAULT;

	kfree(ccid_array);
	return err;
}