static int perf_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct perf_event *event = file->private_data;
	unsigned long user_locked, user_lock_limit;
	struct user_struct *user = current_user();
	struct perf_buffer *rb = NULL;
	unsigned long locked, lock_limit;
	unsigned long vma_size;
	unsigned long nr_pages;
	long user_extra = 0, extra = 0;
	int ret = 0, flags = 0;

	/*
	 * Don't allow mmap() of inherited per-task counters. This would
	 * create a performance issue due to all children writing to the
	 * same rb.
	 */
	if (event->cpu == -1 && event->attr.inherit)
		return -EINVAL;

	if (!(vma->vm_flags & VM_SHARED))
		return -EINVAL;

	ret = security_perf_event_read(event);
	if (ret)
		return ret;

	vma_size = vma->vm_end - vma->vm_start;

	if (vma->vm_pgoff == 0) {
		nr_pages = (vma_size / PAGE_SIZE) - 1;
	} else {
		/*
		 * AUX area mapping: if rb->aux_nr_pages != 0, it's already
		 * mapped, all subsequent mappings should have the same size
		 * and offset. Must be above the normal perf buffer.
		 */
		u64 aux_offset, aux_size;

		if (!event->rb)
			return -EINVAL;

		nr_pages = vma_size / PAGE_SIZE;

		mutex_lock(&event->mmap_mutex);
		ret = -EINVAL;

		rb = event->rb;
		if (!rb)
			goto aux_unlock;

		aux_offset = READ_ONCE(rb->user_page->aux_offset);
		aux_size = READ_ONCE(rb->user_page->aux_size);

		if (aux_offset < perf_data_size(rb) + PAGE_SIZE)
			goto aux_unlock;

		if (aux_offset != vma->vm_pgoff << PAGE_SHIFT)
			goto aux_unlock;

		/* already mapped with a different offset */
		if (rb_has_aux(rb) && rb->aux_pgoff != vma->vm_pgoff)
			goto aux_unlock;

		if (aux_size != vma_size || aux_size != nr_pages * PAGE_SIZE)
			goto aux_unlock;

		/* already mapped with a different size */
		if (rb_has_aux(rb) && rb->aux_nr_pages != nr_pages)
			goto aux_unlock;

		if (!is_power_of_2(nr_pages))
			goto aux_unlock;

		if (!atomic_inc_not_zero(&rb->mmap_count))
			goto aux_unlock;

		if (rb_has_aux(rb)) {
			atomic_inc(&rb->aux_mmap_count);
			ret = 0;
			goto unlock;
		}

		atomic_set(&rb->aux_mmap_count, 1);
		user_extra = nr_pages;

		goto accounting;
	}

	/*
	 * If we have rb pages ensure they're a power-of-two number, so we
	 * can do bitmasks instead of modulo.
	 */
	if (nr_pages != 0 && !is_power_of_2(nr_pages))
		return -EINVAL;

	if (vma_size != PAGE_SIZE * (1 + nr_pages))
		return -EINVAL;

	WARN_ON_ONCE(event->ctx->parent_ctx);
again:
	mutex_lock(&event->mmap_mutex);
	if (event->rb) {
		if (event->rb->nr_pages != nr_pages) {
			ret = -EINVAL;
			goto unlock;
		}

		if (!atomic_inc_not_zero(&event->rb->mmap_count)) {
			/*
			 * Raced against perf_mmap_close() through
			 * perf_event_set_output(). Try again, hope for better
			 * luck.
			 */
			mutex_unlock(&event->mmap_mutex);
			goto again;
		}

		goto unlock;
	}

	user_extra = nr_pages + 1;

accounting:
	user_lock_limit = sysctl_perf_event_mlock >> (PAGE_SHIFT - 10);

	/*
	 * Increase the limit linearly with more CPUs:
	 */
	user_lock_limit *= num_online_cpus();

	user_locked = atomic_long_read(&user->locked_vm);

	/*
	 * sysctl_perf_event_mlock may have changed, so that
	 *     user->locked_vm > user_lock_limit
	 */
	if (user_locked > user_lock_limit)
		user_locked = user_lock_limit;
	user_locked += user_extra;

	if (user_locked > user_lock_limit) {
		/*
		 * charge locked_vm until it hits user_lock_limit;
		 * charge the rest from pinned_vm
		 */
		extra = user_locked - user_lock_limit;
		user_extra -= extra;
	}

	lock_limit = rlimit(RLIMIT_MEMLOCK);
	lock_limit >>= PAGE_SHIFT;
	locked = atomic64_read(&vma->vm_mm->pinned_vm) + extra;

	if ((locked > lock_limit) && perf_is_paranoid() &&
		!capable(CAP_IPC_LOCK)) {
		ret = -EPERM;
		goto unlock;
	}

	WARN_ON(!rb && event->rb);

	if (vma->vm_flags & VM_WRITE)
		flags |= RING_BUFFER_WRITABLE;

	if (!rb) {
		rb = rb_alloc(nr_pages,
			      event->attr.watermark ? event->attr.wakeup_watermark : 0,
			      event->cpu, flags);

		if (!rb) {
			ret = -ENOMEM;
			goto unlock;
		}

		atomic_set(&rb->mmap_count, 1);
		rb->mmap_user = get_current_user();
		rb->mmap_locked = extra;

		ring_buffer_attach(event, rb);

		perf_event_init_userpage(event);
		perf_event_update_userpage(event);
	} else {
		ret = rb_alloc_aux(rb, event, vma->vm_pgoff, nr_pages,
				   event->attr.aux_watermark, flags);
		if (!ret)
			rb->aux_mmap_locked = extra;
	}

unlock:
	if (!ret) {
		atomic_long_add(user_extra, &user->locked_vm);
		atomic64_add(extra, &vma->vm_mm->pinned_vm);

		atomic_inc(&event->mmap_count);
	} else if (rb) {
		atomic_dec(&rb->mmap_count);
	}
aux_unlock:
	mutex_unlock(&event->mmap_mutex);

	/*
	 * Since pinned accounting is per vm we cannot allow fork() to copy our
	 * vma.
	 */
	vma->vm_flags |= VM_DONTCOPY | VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_ops = &perf_mmap_vmops;

	if (event->pmu->event_mapped)
		event->pmu->event_mapped(event, vma->vm_mm);

	return ret;
}