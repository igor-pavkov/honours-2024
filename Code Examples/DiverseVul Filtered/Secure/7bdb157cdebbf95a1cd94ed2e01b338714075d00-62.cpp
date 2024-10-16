static void perf_mmap_open(struct vm_area_struct *vma)
{
	struct perf_event *event = vma->vm_file->private_data;

	atomic_inc(&event->mmap_count);
	atomic_inc(&event->rb->mmap_count);

	if (vma->vm_pgoff)
		atomic_inc(&event->rb->aux_mmap_count);

	if (event->pmu->event_mapped)
		event->pmu->event_mapped(event, vma->vm_mm);
}