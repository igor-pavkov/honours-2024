static int binder_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;
	struct binder_proc *proc = filp->private_data;
	const char *failure_string;

	if (proc->tsk != current->group_leader)
		return -EINVAL;

	if ((vma->vm_end - vma->vm_start) > SZ_4M)
		vma->vm_end = vma->vm_start + SZ_4M;

	binder_debug(BINDER_DEBUG_OPEN_CLOSE,
		     "%s: %d %lx-%lx (%ld K) vma %lx pagep %lx\n",
		     __func__, proc->pid, vma->vm_start, vma->vm_end,
		     (vma->vm_end - vma->vm_start) / SZ_1K, vma->vm_flags,
		     (unsigned long)pgprot_val(vma->vm_page_prot));

	if (vma->vm_flags & FORBIDDEN_MMAP_FLAGS) {
		ret = -EPERM;
		failure_string = "bad vm_flags";
		goto err_bad_arg;
	}
	vma->vm_flags = (vma->vm_flags | VM_DONTCOPY) & ~VM_MAYWRITE;
	vma->vm_ops = &binder_vm_ops;
	vma->vm_private_data = proc;

	ret = binder_alloc_mmap_handler(&proc->alloc, vma);
	if (ret)
		return ret;
	mutex_lock(&proc->files_lock);
	proc->files = get_files_struct(current);
	mutex_unlock(&proc->files_lock);
	return 0;

err_bad_arg:
	pr_err("%s: %d %lx-%lx %s failed %d\n", __func__,
	       proc->pid, vma->vm_start, vma->vm_end, failure_string, ret);
	return ret;
}