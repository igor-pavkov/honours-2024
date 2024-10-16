static bool perf_need_aux_event(struct perf_event *event)
{
	return !!event->attr.aux_output || !!event->attr.aux_sample_size;
}