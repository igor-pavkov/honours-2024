asmlinkage long sys_prctl(int option, unsigned long arg2, unsigned long arg3,
			  unsigned long arg4, unsigned long arg5)
{
	long error;

	error = security_task_prctl(option, arg2, arg3, arg4, arg5);
	if (error)
		return error;

	switch (option) {
		case PR_SET_PDEATHSIG:
			if (!valid_signal(arg2)) {
				error = -EINVAL;
				break;
			}
			current->pdeath_signal = arg2;
			break;
		case PR_GET_PDEATHSIG:
			error = put_user(current->pdeath_signal, (int __user *)arg2);
			break;
		case PR_GET_DUMPABLE:
			error = current->mm->dumpable;
			break;
		case PR_SET_DUMPABLE:
			if (arg2 < 0 || arg2 > 1) {
				error = -EINVAL;
				break;
			}
			current->mm->dumpable = arg2;
			break;

		case PR_SET_UNALIGN:
			error = SET_UNALIGN_CTL(current, arg2);
			break;
		case PR_GET_UNALIGN:
			error = GET_UNALIGN_CTL(current, arg2);
			break;
		case PR_SET_FPEMU:
			error = SET_FPEMU_CTL(current, arg2);
			break;
		case PR_GET_FPEMU:
			error = GET_FPEMU_CTL(current, arg2);
			break;
		case PR_SET_FPEXC:
			error = SET_FPEXC_CTL(current, arg2);
			break;
		case PR_GET_FPEXC:
			error = GET_FPEXC_CTL(current, arg2);
			break;
		case PR_GET_TIMING:
			error = PR_TIMING_STATISTICAL;
			break;
		case PR_SET_TIMING:
			if (arg2 == PR_TIMING_STATISTICAL)
				error = 0;
			else
				error = -EINVAL;
			break;

		case PR_GET_KEEPCAPS:
			if (current->keep_capabilities)
				error = 1;
			break;
		case PR_SET_KEEPCAPS:
			if (arg2 != 0 && arg2 != 1) {
				error = -EINVAL;
				break;
			}
			current->keep_capabilities = arg2;
			break;
		case PR_SET_NAME: {
			struct task_struct *me = current;
			unsigned char ncomm[sizeof(me->comm)];

			ncomm[sizeof(me->comm)-1] = 0;
			if (strncpy_from_user(ncomm, (char __user *)arg2,
						sizeof(me->comm)-1) < 0)
				return -EFAULT;
			set_task_comm(me, ncomm);
			return 0;
		}
		case PR_GET_NAME: {
			struct task_struct *me = current;
			unsigned char tcomm[sizeof(me->comm)];

			get_task_comm(tcomm, me);
			if (copy_to_user((char __user *)arg2, tcomm, sizeof(tcomm)))
				return -EFAULT;
			return 0;
		}
		case PR_GET_ENDIAN:
			error = GET_ENDIAN(current, arg2);
			break;
		case PR_SET_ENDIAN:
			error = SET_ENDIAN(current, arg2);
			break;

		default:
			error = -EINVAL;
			break;
	}
	return error;
}