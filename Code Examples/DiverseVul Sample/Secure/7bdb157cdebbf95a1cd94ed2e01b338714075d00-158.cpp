static void perf_event_cgroup_output(struct perf_event *event, void *data)
{
	struct perf_cgroup_event *cgroup_event = data;
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	u16 header_size = cgroup_event->event_id.header.size;
	int ret;

	if (!perf_event_cgroup_match(event))
		return;

	perf_event_header__init_id(&cgroup_event->event_id.header,
				   &sample, event);
	ret = perf_output_begin(&handle, event,
				cgroup_event->event_id.header.size);
	if (ret)
		goto out;

	perf_output_put(&handle, cgroup_event->event_id);
	__output_copy(&handle, cgroup_event->path, cgroup_event->path_size);

	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
out:
	cgroup_event->event_id.header.size = header_size;
}