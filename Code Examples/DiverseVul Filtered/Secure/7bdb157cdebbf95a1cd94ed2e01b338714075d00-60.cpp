static void perf_event_cgroup(struct cgroup *cgrp)
{
	struct perf_cgroup_event cgroup_event;
	char path_enomem[16] = "//enomem";
	char *pathname;
	size_t size;

	if (!atomic_read(&nr_cgroup_events))
		return;

	cgroup_event = (struct perf_cgroup_event){
		.event_id  = {
			.header = {
				.type = PERF_RECORD_CGROUP,
				.misc = 0,
				.size = sizeof(cgroup_event.event_id),
			},
			.id = cgroup_id(cgrp),
		},
	};

	pathname = kmalloc(PATH_MAX, GFP_KERNEL);
	if (pathname == NULL) {
		cgroup_event.path = path_enomem;
	} else {
		/* just to be sure to have enough space for alignment */
		cgroup_path(cgrp, pathname, PATH_MAX - sizeof(u64));
		cgroup_event.path = pathname;
	}

	/*
	 * Since our buffer works in 8 byte units we need to align our string
	 * size to a multiple of 8. However, we must guarantee the tail end is
	 * zero'd out to avoid leaking random bits to userspace.
	 */
	size = strlen(cgroup_event.path) + 1;
	while (!IS_ALIGNED(size, sizeof(u64)))
		cgroup_event.path[size++] = '\0';

	cgroup_event.event_id.header.size += size;
	cgroup_event.path_size = size;

	perf_iterate_sb(perf_event_cgroup_output,
			&cgroup_event,
			NULL);

	kfree(pathname);
}