COMPAT_SYSCALL_DEFINE2(sigaltstack,
			const compat_stack_t __user *, uss_ptr,
			compat_stack_t __user *, uoss_ptr)
{
	stack_t uss, uoss;
	int ret;
	mm_segment_t seg;

	if (uss_ptr) {
		compat_stack_t uss32;

		memset(&uss, 0, sizeof(stack_t));
		if (copy_from_user(&uss32, uss_ptr, sizeof(compat_stack_t)))
			return -EFAULT;
		uss.ss_sp = compat_ptr(uss32.ss_sp);
		uss.ss_flags = uss32.ss_flags;
		uss.ss_size = uss32.ss_size;
	}
	seg = get_fs();
	set_fs(KERNEL_DS);
	ret = do_sigaltstack((stack_t __force __user *) (uss_ptr ? &uss : NULL),
			     (stack_t __force __user *) &uoss,
			     compat_user_stack_pointer());
	set_fs(seg);
	if (ret >= 0 && uoss_ptr)  {
		if (!access_ok(VERIFY_WRITE, uoss_ptr, sizeof(compat_stack_t)) ||
		    __put_user(ptr_to_compat(uoss.ss_sp), &uoss_ptr->ss_sp) ||
		    __put_user(uoss.ss_flags, &uoss_ptr->ss_flags) ||
		    __put_user(uoss.ss_size, &uoss_ptr->ss_size))
			ret = -EFAULT;
	}
	return ret;
}