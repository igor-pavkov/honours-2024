long arch_ptrace(struct task_struct *child, long request,
		 unsigned long addr, unsigned long data)
{
	int ret = -EPERM;
	void __user *datap = (void __user *) data;

	switch (request) {
	case PTRACE_PEEKTEXT:	/* read word at location addr. */
	case PTRACE_PEEKDATA:
		ret = generic_ptrace_peekdata(child, addr, data);
		break;

	case PTRACE_PEEKUSR:	/* read register specified by addr. */
		ret = ptrace_peekusr(child, addr, datap);
		break;

	case PTRACE_POKETEXT:	/* write the word at location addr. */
	case PTRACE_POKEDATA:
		ret = generic_ptrace_pokedata(child, addr, data);
		break;

	case PTRACE_POKEUSR:	/* write register specified by addr. */
		ret = ptrace_pokeusr(child, addr, data);
		break;

	case PTRACE_GETREGS:
		ret = ptrace_getregs(child, datap);
		break;

	case PTRACE_SETREGS:
		ret = ptrace_setregs(child, datap);
		break;

	case PTRACE_GETXTREGS:
		ret = ptrace_getxregs(child, datap);
		break;

	case PTRACE_SETXTREGS:
		ret = ptrace_setxregs(child, datap);
		break;

	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}