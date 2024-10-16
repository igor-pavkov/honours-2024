void perf_event_text_poke(const void *addr, const void *old_bytes,
			  size_t old_len, const void *new_bytes, size_t new_len)
{
	struct perf_text_poke_event text_poke_event;
	size_t tot, pad;

	if (!atomic_read(&nr_text_poke_events))
		return;

	tot  = sizeof(text_poke_event.old_len) + old_len;
	tot += sizeof(text_poke_event.new_len) + new_len;
	pad  = ALIGN(tot, sizeof(u64)) - tot;

	text_poke_event = (struct perf_text_poke_event){
		.old_bytes    = old_bytes,
		.new_bytes    = new_bytes,
		.pad          = pad,
		.old_len      = old_len,
		.new_len      = new_len,
		.event_id  = {
			.header = {
				.type = PERF_RECORD_TEXT_POKE,
				.misc = PERF_RECORD_MISC_KERNEL,
				.size = sizeof(text_poke_event.event_id) + tot + pad,
			},
			.addr = (unsigned long)addr,
		},
	};

	perf_iterate_sb(perf_event_text_poke_output, &text_poke_event, NULL);
}