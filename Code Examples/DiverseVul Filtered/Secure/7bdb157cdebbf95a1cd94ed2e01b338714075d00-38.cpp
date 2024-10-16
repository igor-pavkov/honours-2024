static int perf_event_ksymbol_match(struct perf_event *event)
{
	return event->attr.ksymbol;
}