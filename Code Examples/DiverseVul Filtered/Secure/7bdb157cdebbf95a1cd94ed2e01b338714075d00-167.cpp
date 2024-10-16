static void perf_pending_event_disable(struct perf_event *event)
{
	int cpu = READ_ONCE(event->pending_disable);

	if (cpu < 0)
		return;

	if (cpu == smp_processor_id()) {
		WRITE_ONCE(event->pending_disable, -1);
		perf_event_disable_local(event);
		return;
	}

	/*
	 *  CPU-A			CPU-B
	 *
	 *  perf_event_disable_inatomic()
	 *    @pending_disable = CPU-A;
	 *    irq_work_queue();
	 *
	 *  sched-out
	 *    @pending_disable = -1;
	 *
	 *				sched-in
	 *				perf_event_disable_inatomic()
	 *				  @pending_disable = CPU-B;
	 *				  irq_work_queue(); // FAILS
	 *
	 *  irq_work_run()
	 *    perf_pending_event()
	 *
	 * But the event runs on CPU-B and wants disabling there.
	 */
	irq_work_queue_on(&event->pending, cpu);
}