long perf_pmu_snapshot_aux(struct perf_buffer *rb,
			   struct perf_event *event,
			   struct perf_output_handle *handle,
			   unsigned long size)
{
	unsigned long flags;
	long ret;

	/*
	 * Normal ->start()/->stop() callbacks run in IRQ mode in scheduler
	 * paths. If we start calling them in NMI context, they may race with
	 * the IRQ ones, that is, for example, re-starting an event that's just
	 * been stopped, which is why we're using a separate callback that
	 * doesn't change the event state.
	 *
	 * IRQs need to be disabled to prevent IPIs from racing with us.
	 */
	local_irq_save(flags);
	/*
	 * Guard against NMI hits inside the critical section;
	 * see also perf_prepare_sample_aux().
	 */
	WRITE_ONCE(rb->aux_in_sampling, 1);
	barrier();

	ret = event->pmu->snapshot_aux(event, handle, size);

	barrier();
	WRITE_ONCE(rb->aux_in_sampling, 0);
	local_irq_restore(flags);

	return ret;
}