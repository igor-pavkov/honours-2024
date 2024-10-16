int kvm_io_bus_write(struct kvm *kvm, enum kvm_bus bus_idx, gpa_t addr,
		     int len, const void *val)
{
	struct kvm_io_bus *bus;
	struct kvm_io_range range;
	int r;

	range = (struct kvm_io_range) {
		.addr = addr,
		.len = len,
	};

	bus = srcu_dereference(kvm->buses[bus_idx], &kvm->srcu);
	r = __kvm_io_bus_write(bus, &range, val);
	return r < 0 ? r : 0;
}