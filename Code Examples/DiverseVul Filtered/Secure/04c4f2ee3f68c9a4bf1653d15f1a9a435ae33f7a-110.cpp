static int init_rmode_tss(struct kvm *kvm, void __user *ua)
{
	const void *zero_page = (const void *) __va(page_to_phys(ZERO_PAGE(0)));
	u16 data;
	int i;

	for (i = 0; i < 3; i++) {
		if (__copy_to_user(ua + PAGE_SIZE * i, zero_page, PAGE_SIZE))
			return -EFAULT;
	}

	data = TSS_BASE_SIZE + TSS_REDIRECTION_SIZE;
	if (__copy_to_user(ua + TSS_IOPB_BASE_OFFSET, &data, sizeof(u16)))
		return -EFAULT;

	data = ~0;
	if (__copy_to_user(ua + RMODE_TSS_SIZE - 1, &data, sizeof(u8)))
		return -EFAULT;

	return 0;
}