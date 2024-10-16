void perf_event_namespaces(struct task_struct *task)
{
	struct perf_namespaces_event namespaces_event;
	struct perf_ns_link_info *ns_link_info;

	if (!atomic_read(&nr_namespaces_events))
		return;

	namespaces_event = (struct perf_namespaces_event){
		.task	= task,
		.event_id  = {
			.header = {
				.type = PERF_RECORD_NAMESPACES,
				.misc = 0,
				.size = sizeof(namespaces_event.event_id),
			},
			/* .pid */
			/* .tid */
			.nr_namespaces = NR_NAMESPACES,
			/* .link_info[NR_NAMESPACES] */
		},
	};

	ns_link_info = namespaces_event.event_id.link_info;

	perf_fill_ns_link_info(&ns_link_info[MNT_NS_INDEX],
			       task, &mntns_operations);

#ifdef CONFIG_USER_NS
	perf_fill_ns_link_info(&ns_link_info[USER_NS_INDEX],
			       task, &userns_operations);
#endif
#ifdef CONFIG_NET_NS
	perf_fill_ns_link_info(&ns_link_info[NET_NS_INDEX],
			       task, &netns_operations);
#endif
#ifdef CONFIG_UTS_NS
	perf_fill_ns_link_info(&ns_link_info[UTS_NS_INDEX],
			       task, &utsns_operations);
#endif
#ifdef CONFIG_IPC_NS
	perf_fill_ns_link_info(&ns_link_info[IPC_NS_INDEX],
			       task, &ipcns_operations);
#endif
#ifdef CONFIG_PID_NS
	perf_fill_ns_link_info(&ns_link_info[PID_NS_INDEX],
			       task, &pidns_operations);
#endif
#ifdef CONFIG_CGROUPS
	perf_fill_ns_link_info(&ns_link_info[CGROUP_NS_INDEX],
			       task, &cgroupns_operations);
#endif

	perf_iterate_sb(perf_event_namespaces_output,
			&namespaces_event,
			NULL);
}