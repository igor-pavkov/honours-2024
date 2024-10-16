void perf_event_bpf_event(struct bpf_prog *prog,
			  enum perf_bpf_event_type type,
			  u16 flags)
{
	struct perf_bpf_event bpf_event;

	if (type <= PERF_BPF_EVENT_UNKNOWN ||
	    type >= PERF_BPF_EVENT_MAX)
		return;

	switch (type) {
	case PERF_BPF_EVENT_PROG_LOAD:
	case PERF_BPF_EVENT_PROG_UNLOAD:
		if (atomic_read(&nr_ksymbol_events))
			perf_event_bpf_emit_ksymbols(prog, type);
		break;
	default:
		break;
	}

	if (!atomic_read(&nr_bpf_events))
		return;

	bpf_event = (struct perf_bpf_event){
		.prog = prog,
		.event_id = {
			.header = {
				.type = PERF_RECORD_BPF_EVENT,
				.size = sizeof(bpf_event.event_id),
			},
			.type = type,
			.flags = flags,
			.id = prog->aux->id,
		},
	};

	BUILD_BUG_ON(BPF_TAG_SIZE % sizeof(u64));

	memcpy(bpf_event.event_id.tag, prog->tag, BPF_TAG_SIZE);
	perf_iterate_sb(perf_event_bpf_output, &bpf_event, NULL);
}