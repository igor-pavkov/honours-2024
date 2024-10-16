static bool exclusive_event_installable(struct perf_event *event,
					struct perf_event_context *ctx)
{
	struct perf_event *iter_event;
	struct pmu *pmu = event->pmu;

	lockdep_assert_held(&ctx->mutex);

	if (!is_exclusive_pmu(pmu))
		return true;

	list_for_each_entry(iter_event, &ctx->event_list, event_entry) {
		if (exclusive_event_match(iter_event, event))
			return false;
	}

	return true;
}