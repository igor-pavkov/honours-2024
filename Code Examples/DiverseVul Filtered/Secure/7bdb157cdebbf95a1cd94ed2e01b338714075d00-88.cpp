static unsigned long perf_prepare_sample_aux(struct perf_event *event,
					  struct perf_sample_data *data,
					  size_t size)
{
	struct perf_event *sampler = event->aux_event;
	struct perf_buffer *rb;

	data->aux_size = 0;

	if (!sampler)
		goto out;

	if (WARN_ON_ONCE(READ_ONCE(sampler->state) != PERF_EVENT_STATE_ACTIVE))
		goto out;

	if (WARN_ON_ONCE(READ_ONCE(sampler->oncpu) != smp_processor_id()))
		goto out;

	rb = ring_buffer_get(sampler->parent ? sampler->parent : sampler);
	if (!rb)
		goto out;

	/*
	 * If this is an NMI hit inside sampling code, don't take
	 * the sample. See also perf_aux_sample_output().
	 */
	if (READ_ONCE(rb->aux_in_sampling)) {
		data->aux_size = 0;
	} else {
		size = min_t(size_t, size, perf_aux_size(rb));
		data->aux_size = ALIGN(size, sizeof(u64));
	}
	ring_buffer_put(rb);

out:
	return data->aux_size;
}