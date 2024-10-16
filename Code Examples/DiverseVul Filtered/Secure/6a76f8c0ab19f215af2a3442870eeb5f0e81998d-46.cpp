static int ftrace_update_code(struct module *mod)
{
	struct ftrace_page *pg;
	struct dyn_ftrace *p;
	cycle_t start, stop;
	unsigned long ref = 0;
	int i;

	/*
	 * When adding a module, we need to check if tracers are
	 * currently enabled and if they are set to trace all functions.
	 * If they are, we need to enable the module functions as well
	 * as update the reference counts for those function records.
	 */
	if (mod) {
		struct ftrace_ops *ops;

		for (ops = ftrace_ops_list;
		     ops != &ftrace_list_end; ops = ops->next) {
			if (ops->flags & FTRACE_OPS_FL_ENABLED &&
			    ops_traces_mod(ops))
				ref++;
		}
	}

	start = ftrace_now(raw_smp_processor_id());
	ftrace_update_cnt = 0;

	for (pg = ftrace_new_pgs; pg; pg = pg->next) {

		for (i = 0; i < pg->index; i++) {
			/* If something went wrong, bail without enabling anything */
			if (unlikely(ftrace_disabled))
				return -1;

			p = &pg->records[i];
			p->flags = ref;

			/*
			 * Do the initial record conversion from mcount jump
			 * to the NOP instructions.
			 */
			if (!ftrace_code_disable(mod, p))
				break;

			ftrace_update_cnt++;

			/*
			 * If the tracing is enabled, go ahead and enable the record.
			 *
			 * The reason not to enable the record immediatelly is the
			 * inherent check of ftrace_make_nop/ftrace_make_call for
			 * correct previous instructions.  Making first the NOP
			 * conversion puts the module to the correct state, thus
			 * passing the ftrace_make_call check.
			 */
			if (ftrace_start_up && ref) {
				int failed = __ftrace_replace_code(p, 1);
				if (failed)
					ftrace_bug(failed, p->ip);
			}
		}
	}

	ftrace_new_pgs = NULL;

	stop = ftrace_now(raw_smp_processor_id());
	ftrace_update_time = stop - start;
	ftrace_update_tot_cnt += ftrace_update_cnt;

	return 0;
}