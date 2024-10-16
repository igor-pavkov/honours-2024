static void perf_log_itrace_start(struct perf_event *event)
{
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	struct perf_aux_event {
		struct perf_event_header        header;
		u32				pid;
		u32				tid;
	} rec;
	int ret;

	if (event->parent)
		event = event->parent;

	if (!(event->pmu->capabilities & PERF_PMU_CAP_ITRACE) ||
	    event->attach_state & PERF_ATTACH_ITRACE)
		return;

	rec.header.type	= PERF_RECORD_ITRACE_START;
	rec.header.misc	= 0;
	rec.header.size	= sizeof(rec);
	rec.pid	= perf_event_pid(event, current);
	rec.tid	= perf_event_tid(event, current);

	perf_event_header__init_id(&rec.header, &sample, event);
	ret = perf_output_begin(&handle, event, rec.header.size);

	if (ret)
		return;

	perf_output_put(&handle, rec);
	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
}