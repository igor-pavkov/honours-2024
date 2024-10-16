static void perf_aux_sample_output(struct perf_event *event,
				   struct perf_output_handle *handle,
				   struct perf_sample_data *data)
{
	struct perf_event *sampler = event->aux_event;
	struct perf_buffer *rb;
	unsigned long pad;
	long size;

	if (WARN_ON_ONCE(!sampler || !data->aux_size))
		return;

	rb = ring_buffer_get(sampler->parent ? sampler->parent : sampler);
	if (!rb)
		return;

	size = perf_pmu_snapshot_aux(rb, sampler, handle, data->aux_size);

	/*
	 * An error here means that perf_output_copy() failed (returned a
	 * non-zero surplus that it didn't copy), which in its current
	 * enlightened implementation is not possible. If that changes, we'd
	 * like to know.
	 */
	if (WARN_ON_ONCE(size < 0))
		goto out_put;

	/*
	 * The pad comes from ALIGN()ing data->aux_size up to u64 in
	 * perf_prepare_sample_aux(), so should not be more than that.
	 */
	pad = data->aux_size - size;
	if (WARN_ON_ONCE(pad >= sizeof(u64)))
		pad = 8;

	if (pad) {
		u64 zero = 0;
		perf_output_copy(handle, &zero, pad);
	}

out_put:
	ring_buffer_put(rb);
}