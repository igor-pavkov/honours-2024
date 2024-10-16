static inline void perf_tp_register(void)
{
	perf_pmu_register(&perf_tracepoint, "tracepoint", PERF_TYPE_TRACEPOINT);
#ifdef CONFIG_KPROBE_EVENTS
	perf_pmu_register(&perf_kprobe, "kprobe", -1);
#endif
#ifdef CONFIG_UPROBE_EVENTS
	perf_pmu_register(&perf_uprobe, "uprobe", -1);
#endif
}