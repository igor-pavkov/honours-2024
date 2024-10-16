static void perf_event_text_poke_output(struct perf_event *event, void *data)
{
	struct perf_text_poke_event *text_poke_event = data;
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	u64 padding = 0;
	int ret;

	if (!perf_event_text_poke_match(event))
		return;

	perf_event_header__init_id(&text_poke_event->event_id.header, &sample, event);

	ret = perf_output_begin(&handle, event, text_poke_event->event_id.header.size);
	if (ret)
		return;

	perf_output_put(&handle, text_poke_event->event_id);
	perf_output_put(&handle, text_poke_event->old_len);
	perf_output_put(&handle, text_poke_event->new_len);

	__output_copy(&handle, text_poke_event->old_bytes, text_poke_event->old_len);
	__output_copy(&handle, text_poke_event->new_bytes, text_poke_event->new_len);

	if (text_poke_event->pad)
		__output_copy(&handle, &padding, text_poke_event->pad);

	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
}