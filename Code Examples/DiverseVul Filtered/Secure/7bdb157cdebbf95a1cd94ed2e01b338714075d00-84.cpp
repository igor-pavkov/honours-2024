static void bpf_overflow_handler(struct perf_event *event,
				 struct perf_sample_data *data,
				 struct pt_regs *regs)
{
	struct bpf_perf_event_data_kern ctx = {
		.data = data,
		.event = event,
	};
	int ret = 0;

	ctx.regs = perf_arch_bpf_user_pt_regs(regs);
	if (unlikely(__this_cpu_inc_return(bpf_prog_active) != 1))
		goto out;
	rcu_read_lock();
	ret = BPF_PROG_RUN(event->prog, &ctx);
	rcu_read_unlock();
out:
	__this_cpu_dec(bpf_prog_active);
	if (!ret)
		return;

	event->orig_overflow_handler(event, data, regs);
}