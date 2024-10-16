int kvm_io_bus_insert_dev(struct kvm_io_bus *bus, struct kvm_io_device *dev,
			  gpa_t addr, int len)
{
	bus->range[bus->dev_count++] = (struct kvm_io_range) {
		.addr = addr,
		.len = len,
		.dev = dev,
	};

	sort(bus->range, bus->dev_count, sizeof(struct kvm_io_range),
		kvm_io_bus_sort_cmp, NULL);

	return 0;
}