static void perf_event_ksymbol_output(struct perf_event *event, void *data)
{
	struct perf_ksymbol_event *ksymbol_event = data;
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	int ret;

	if (!perf_event_ksymbol_match(event))
		return;

	perf_event_header__init_id(&ksymbol_event->event_id.header,
				   &sample, event);
	ret = perf_output_begin(&handle, event,
				ksymbol_event->event_id.header.size);
	if (ret)
		return;

	perf_output_put(&handle, ksymbol_event->event_id);
	__output_copy(&handle, ksymbol_event->name, ksymbol_event->name_len);
	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
}