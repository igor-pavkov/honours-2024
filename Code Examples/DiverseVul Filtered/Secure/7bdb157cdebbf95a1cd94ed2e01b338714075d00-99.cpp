static int perf_event_text_poke_match(struct perf_event *event)
{
	return event->attr.text_poke;
}