int kvm_io_bus_read_cookie(struct kvm *kvm, enum kvm_bus bus_idx, gpa_t addr,
			   int len, void *val, long cookie)
{
	struct kvm_io_bus *bus;
	struct kvm_io_range range;

	range = (struct kvm_io_range) {
		.addr = addr,
		.len = len,
	};

	bus = srcu_dereference(kvm->buses[bus_idx], &kvm->srcu);

	/* First try the device referenced by cookie. */
	if ((cookie >= 0) && (cookie < bus->dev_count) &&
	    (kvm_io_bus_cmp(&range, &bus->range[cookie]) == 0))
		if (!kvm_iodevice_read(bus->range[cookie].dev, addr, len,
				       val))
			return cookie;

	/*
	 * cookie contained garbage; fall back to search and return the
	 * correct cookie value.
	 */
	return __kvm_io_bus_read(bus, &range, val);
}