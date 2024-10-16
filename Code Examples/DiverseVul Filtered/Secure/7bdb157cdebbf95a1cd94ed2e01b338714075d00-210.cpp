static void perf_event_namespaces_output(struct perf_event *event,
					 void *data)
{
	struct perf_namespaces_event *namespaces_event = data;
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	u16 header_size = namespaces_event->event_id.header.size;
	int ret;

	if (!perf_event_namespaces_match(event))
		return;

	perf_event_header__init_id(&namespaces_event->event_id.header,
				   &sample, event);
	ret = perf_output_begin(&handle, event,
				namespaces_event->event_id.header.size);
	if (ret)
		goto out;

	namespaces_event->event_id.pid = perf_event_pid(event,
							namespaces_event->task);
	namespaces_event->event_id.tid = perf_event_tid(event,
							namespaces_event->task);

	perf_output_put(&handle, namespaces_event->event_id);

	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
out:
	namespaces_event->event_id.header.size = header_size;
}