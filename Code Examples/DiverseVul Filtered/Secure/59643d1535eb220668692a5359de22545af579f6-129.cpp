int ring_buffer_poll_wait(struct ring_buffer *buffer, int cpu,
			  struct file *filp, poll_table *poll_table)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct rb_irq_work *work;

	if (cpu == RING_BUFFER_ALL_CPUS)
		work = &buffer->irq_work;
	else {
		if (!cpumask_test_cpu(cpu, buffer->cpumask))
			return -EINVAL;

		cpu_buffer = buffer->buffers[cpu];
		work = &cpu_buffer->irq_work;
	}

	poll_wait(filp, &work->waiters, poll_table);
	work->waiters_pending = true;
	/*
	 * There's a tight race between setting the waiters_pending and
	 * checking if the ring buffer is empty.  Once the waiters_pending bit
	 * is set, the next event will wake the task up, but we can get stuck
	 * if there's only a single event in.
	 *
	 * FIXME: Ideally, we need a memory barrier on the writer side as well,
	 * but adding a memory barrier to all events will cause too much of a
	 * performance hit in the fast path.  We only need a memory barrier when
	 * the buffer goes from empty to having content.  But as this race is
	 * extremely small, and it's not a problem if another event comes in, we
	 * will fix it later.
	 */
	smp_mb();

	if ((cpu == RING_BUFFER_ALL_CPUS && !ring_buffer_empty(buffer)) ||
	    (cpu != RING_BUFFER_ALL_CPUS && !ring_buffer_empty_cpu(buffer, cpu)))
		return POLLIN | POLLRDNORM;
	return 0;
}