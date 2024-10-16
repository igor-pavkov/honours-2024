static void perf_cgroup_switch(struct task_struct *task, int mode)
{
	struct perf_cpu_context *cpuctx;
	struct list_head *list;
	unsigned long flags;

	/*
	 * Disable interrupts and preemption to avoid this CPU's
	 * cgrp_cpuctx_entry to change under us.
	 */
	local_irq_save(flags);

	list = this_cpu_ptr(&cgrp_cpuctx_list);
	list_for_each_entry(cpuctx, list, cgrp_cpuctx_entry) {
		WARN_ON_ONCE(cpuctx->ctx.nr_cgroups == 0);

		perf_ctx_lock(cpuctx, cpuctx->task_ctx);
		perf_pmu_disable(cpuctx->ctx.pmu);

		if (mode & PERF_CGROUP_SWOUT) {
			cpu_ctx_sched_out(cpuctx, EVENT_ALL);
			/*
			 * must not be done before ctxswout due
			 * to event_filter_match() in event_sched_out()
			 */
			cpuctx->cgrp = NULL;
		}

		if (mode & PERF_CGROUP_SWIN) {
			WARN_ON_ONCE(cpuctx->cgrp);
			/*
			 * set cgrp before ctxsw in to allow
			 * event_filter_match() to not have to pass
			 * task around
			 * we pass the cpuctx->ctx to perf_cgroup_from_task()
			 * because cgorup events are only per-cpu
			 */
			cpuctx->cgrp = perf_cgroup_from_task(task,
							     &cpuctx->ctx);
			cpu_ctx_sched_in(cpuctx, EVENT_ALL, task);
		}
		perf_pmu_enable(cpuctx->ctx.pmu);
		perf_ctx_unlock(cpuctx, cpuctx->task_ctx);
	}

	local_irq_restore(flags);
}