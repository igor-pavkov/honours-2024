static inline bool has_extended_regs(struct perf_event *event)
{
	return (event->attr.sample_regs_user & PERF_REG_EXTENDED_MASK) ||
	       (event->attr.sample_regs_intr & PERF_REG_EXTENDED_MASK);
}