SYSCALL_DEFINE3(ioprio_set, int, which, int, who, int, ioprio)
{
	int class = IOPRIO_PRIO_CLASS(ioprio);
	int data = IOPRIO_PRIO_DATA(ioprio);
	struct task_struct *p, *g;
	struct user_struct *user;
	struct pid *pgrp;
	kuid_t uid;
	int ret;

	switch (class) {
		case IOPRIO_CLASS_RT:
			if (!capable(CAP_SYS_ADMIN))
				return -EPERM;
			/* fall through, rt has prio field too */
		case IOPRIO_CLASS_BE:
			if (data >= IOPRIO_BE_NR || data < 0)
				return -EINVAL;

			break;
		case IOPRIO_CLASS_IDLE:
			break;
		case IOPRIO_CLASS_NONE:
			if (data)
				return -EINVAL;
			break;
		default:
			return -EINVAL;
	}

	ret = -ESRCH;
	rcu_read_lock();
	switch (which) {
		case IOPRIO_WHO_PROCESS:
			if (!who)
				p = current;
			else
				p = find_task_by_vpid(who);
			if (p)
				ret = set_task_ioprio(p, ioprio);
			break;
		case IOPRIO_WHO_PGRP:
			if (!who)
				pgrp = task_pgrp(current);
			else
				pgrp = find_vpid(who);
			do_each_pid_thread(pgrp, PIDTYPE_PGID, p) {
				ret = set_task_ioprio(p, ioprio);
				if (ret)
					break;
			} while_each_pid_thread(pgrp, PIDTYPE_PGID, p);
			break;
		case IOPRIO_WHO_USER:
			uid = make_kuid(current_user_ns(), who);
			if (!uid_valid(uid))
				break;
			if (!who)
				user = current_user();
			else
				user = find_user(uid);

			if (!user)
				break;

			do_each_thread(g, p) {
				if (!uid_eq(task_uid(p), uid) ||
				    !task_pid_vnr(p))
					continue;
				ret = set_task_ioprio(p, ioprio);
				if (ret)
					goto free_uid;
			} while_each_thread(g, p);
free_uid:
			if (who)
				free_uid(user);
			break;
		default:
			ret = -EINVAL;
	}

	rcu_read_unlock();
	return ret;
}