static inline int pit_in_range(gpa_t addr)
{
	return ((addr >= KVM_PIT_BASE_ADDRESS) &&
		(addr < KVM_PIT_BASE_ADDRESS + KVM_PIT_MEM_LENGTH));
}