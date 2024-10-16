static void perf_event_bpf_emit_ksymbols(struct bpf_prog *prog,
					 enum perf_bpf_event_type type)
{
	bool unregister = type == PERF_BPF_EVENT_PROG_UNLOAD;
	int i;

	if (prog->aux->func_cnt == 0) {
		perf_event_ksymbol(PERF_RECORD_KSYMBOL_TYPE_BPF,
				   (u64)(unsigned long)prog->bpf_func,
				   prog->jited_len, unregister,
				   prog->aux->ksym.name);
	} else {
		for (i = 0; i < prog->aux->func_cnt; i++) {
			struct bpf_prog *subprog = prog->aux->func[i];

			perf_event_ksymbol(
				PERF_RECORD_KSYMBOL_TYPE_BPF,
				(u64)(unsigned long)subprog->bpf_func,
				subprog->jited_len, unregister,
				prog->aux->ksym.name);
		}
	}
}