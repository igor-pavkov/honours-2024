static inline int kvm_io_bus_cmp(const struct kvm_io_range *r1,
                                 const struct kvm_io_range *r2)
{
	if (r1->addr < r2->addr)
		return -1;
	if (r1->addr + r1->len > r2->addr + r2->len)
		return 1;
	return 0;
}