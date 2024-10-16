int sel_loadlut(char __user *p)
{
	return copy_from_user(inwordLut, (u32 __user *)(p+4), 32) ? -EFAULT : 0;
}