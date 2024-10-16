static void __user *u64_to_uptr(u64 value)
{
	if (is_compat_task())
		return compat_ptr(value);
	else
		return (void __user *)(unsigned long)value;
}