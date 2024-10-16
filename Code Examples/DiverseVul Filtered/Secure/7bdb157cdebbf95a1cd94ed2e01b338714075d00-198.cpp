static void perf_event_bpf_output(struct perf_event *event, void *data)
{
	struct perf_bpf_event *bpf_event = data;
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	int ret;

	if (!perf_event_bpf_match(event))
		return;

	perf_event_header__init_id(&bpf_event->event_id.header,
				   &sample, event);
	ret = perf_output_begin(&handle, event,
				bpf_event->event_id.header.size);
	if (ret)
		return;

	perf_output_put(&handle, bpf_event->event_id);
	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
}