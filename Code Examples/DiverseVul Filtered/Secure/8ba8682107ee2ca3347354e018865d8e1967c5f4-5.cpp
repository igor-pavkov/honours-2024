SYSCALL_DEFINE2(ioprio_get, int, which, int, who)
{
	struct task_struct *g, *p;
	struct user_struct *user;
	struct pid *pgrp;
	kuid_t uid;
	int ret = -ESRCH;
	int tmpio;

	rcu_read_lock();
	switch (which) {
		case IOPRIO_WHO_PROCESS:
			if (!who)
				p = current;
			else
				p = find_task_by_vpid(who);
			if (p)
				ret = get_task_ioprio(p);
			break;
		case IOPRIO_WHO_PGRP:
			if (!who)
				pgrp = task_pgrp(current);
			else
				pgrp = find_vpid(who);
			do_each_pid_thread(pgrp, PIDTYPE_PGID, p) {
				tmpio = get_task_ioprio(p);
				if (tmpio < 0)
					continue;
				if (ret == -ESRCH)
					ret = tmpio;
				else
					ret = ioprio_best(ret, tmpio);
			} while_each_pid_thread(pgrp, PIDTYPE_PGID, p);
			break;
		case IOPRIO_WHO_USER:
			uid = make_kuid(current_user_ns(), who);
			if (!who)
				user = current_user();
			else
				user = find_user(uid);

			if (!user)
				break;

			do_each_thread(g, p) {
				if (!uid_eq(task_uid(p), user->uid) ||
				    !task_pid_vnr(p))
					continue;
				tmpio = get_task_ioprio(p);
				if (tmpio < 0)
					continue;
				if (ret == -ESRCH)
					ret = tmpio;
				else
					ret = ioprio_best(ret, tmpio);
			} while_each_thread(g, p);

			if (who)
				free_uid(user);
			break;
		default:
			ret = -EINVAL;
	}

	rcu_read_unlock();
	return ret;
}