void *kvm_kvzalloc(unsigned long size)
{
	if (size > PAGE_SIZE)
		return vzalloc(size);
	else
		return kzalloc(size, GFP_KERNEL);
}