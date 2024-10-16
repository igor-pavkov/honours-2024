static int __perf_event_read_cpu(struct perf_event *event, int event_cpu)
{
	u16 local_pkg, event_pkg;

	if (event->group_caps & PERF_EV_CAP_READ_ACTIVE_PKG) {
		int local_cpu = smp_processor_id();

		event_pkg = topology_physical_package_id(event_cpu);
		local_pkg = topology_physical_package_id(local_cpu);

		if (event_pkg == local_pkg)
			return local_cpu;
	}

	return event_cpu;
}