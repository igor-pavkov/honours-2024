static int FNAME(walk_addr)(struct guest_walker *walker,
			    struct kvm_vcpu *vcpu, gpa_t addr, u64 access)
{
	return FNAME(walk_addr_generic)(walker, vcpu, vcpu->arch.mmu, addr,
					access);
}