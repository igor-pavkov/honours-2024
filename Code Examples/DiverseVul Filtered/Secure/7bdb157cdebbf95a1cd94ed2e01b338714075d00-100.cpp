static int perf_event_set_bpf_prog(struct perf_event *event, u32 prog_fd)
{
	bool is_kprobe, is_tracepoint, is_syscall_tp;
	struct bpf_prog *prog;
	int ret;

	if (!perf_event_is_tracing(event))
		return perf_event_set_bpf_handler(event, prog_fd);

	is_kprobe = event->tp_event->flags & TRACE_EVENT_FL_UKPROBE;
	is_tracepoint = event->tp_event->flags & TRACE_EVENT_FL_TRACEPOINT;
	is_syscall_tp = is_syscall_trace_event(event->tp_event);
	if (!is_kprobe && !is_tracepoint && !is_syscall_tp)
		/* bpf programs can only be attached to u/kprobe or tracepoint */
		return -EINVAL;

	prog = bpf_prog_get(prog_fd);
	if (IS_ERR(prog))
		return PTR_ERR(prog);

	if ((is_kprobe && prog->type != BPF_PROG_TYPE_KPROBE) ||
	    (is_tracepoint && prog->type != BPF_PROG_TYPE_TRACEPOINT) ||
	    (is_syscall_tp && prog->type != BPF_PROG_TYPE_TRACEPOINT)) {
		/* valid fd, but invalid bpf program type */
		bpf_prog_put(prog);
		return -EINVAL;
	}

	/* Kprobe override only works for kprobes, not uprobes. */
	if (prog->kprobe_override &&
	    !(event->tp_event->flags & TRACE_EVENT_FL_KPROBE)) {
		bpf_prog_put(prog);
		return -EINVAL;
	}

	if (is_tracepoint || is_syscall_tp) {
		int off = trace_event_get_offsets(event->tp_event);

		if (prog->aux->max_ctx_offset > off) {
			bpf_prog_put(prog);
			return -EACCES;
		}
	}

	ret = perf_event_attach_bpf_prog(event, prog);
	if (ret)
		bpf_prog_put(prog);
	return ret;
}