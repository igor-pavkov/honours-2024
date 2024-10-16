static int copy_fs(unsigned long clone_flags, struct task_struct *tsk)
{
	if (clone_flags & CLONE_FS) {
		atomic_inc(&current->fs->count);
		return 0;
	}
	tsk->fs = __copy_fs_struct(current->fs);
	if (!tsk->fs)
		return -ENOMEM;
	return 0;
}