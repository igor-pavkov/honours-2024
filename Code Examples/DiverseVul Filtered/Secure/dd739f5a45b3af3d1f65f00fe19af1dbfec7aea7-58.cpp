static int cmd_tasks(void *data, const char *input) {
	RCore *core = (RCore*) data;
	switch (input[0]) {
	case '\0': // "&"
	case 'j': // "&j"
		r_core_task_list (core, *input);
		break;
	case 'b': { // "&b"
		if (r_sandbox_enable (0)) {
			eprintf ("This command is disabled in sandbox mode\n");
			return 0;
		}
		int tid = r_num_math (core->num, input + 1);
		if (tid) {
			r_core_task_break (core, tid);
		}
		break;
	}
	case '&': { // "&&"
		if (r_sandbox_enable (0)) {
			eprintf ("This command is disabled in sandbox mode\n");
			return 0;
		}
		int tid = r_num_math (core->num, input + 1);
		r_core_task_join (core, core->current_task, tid ? tid : -1);
		break;
	}
	case '=': { // "&="
		// r_core_task_list (core, '=');
		int tid = r_num_math (core->num, input + 1);
		if (tid) {
			RCoreTask *task = r_core_task_get_incref (core, tid);
			if (task) {
				if (task->res) {
					r_cons_println (task->res);
				}
				r_core_task_decref (task);
			} else {
				eprintf ("Cannot find task\n");
			}
		}
		break;
	}
	case '-': // "&-"
		if (r_sandbox_enable (0)) {
			eprintf ("This command is disabled in sandbox mode\n");
			return 0;
		}
		if (input[1] == '*') {
			r_core_task_del_all_done (core);
		} else {
			r_core_task_del (core, r_num_math (core->num, input + 1));
		}
		break;
	case '?': // "&?"
	default:
		helpCmdTasks (core);
		break;
	case ' ': // "& "
	case '_': // "&_"
	case 't': { // "&t"
		if (r_sandbox_enable (0)) {
			eprintf ("This command is disabled in sandbox mode\n");
			return 0;
		}
		RCoreTask *task = r_core_task_new (core, true, input + 1, NULL, core);
		if (!task) {
			break;
		}
		task->transient = input[0] == 't';
		r_core_task_enqueue (core, task);
		break;
	}
	}
	return 0;
}