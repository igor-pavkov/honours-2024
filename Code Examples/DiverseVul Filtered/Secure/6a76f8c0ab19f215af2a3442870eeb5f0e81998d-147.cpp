int register_ftrace_command(struct ftrace_func_command *cmd)
{
	struct ftrace_func_command *p;
	int ret = 0;

	mutex_lock(&ftrace_cmd_mutex);
	list_for_each_entry(p, &ftrace_commands, list) {
		if (strcmp(cmd->name, p->name) == 0) {
			ret = -EBUSY;
			goto out_unlock;
		}
	}
	list_add(&cmd->list, &ftrace_commands);
 out_unlock:
	mutex_unlock(&ftrace_cmd_mutex);

	return ret;
}