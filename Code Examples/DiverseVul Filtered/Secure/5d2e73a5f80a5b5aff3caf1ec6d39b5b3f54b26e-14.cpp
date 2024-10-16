static int zr364xx_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	int ret;

	if (!cam) {
		DBG("%s: cam == NULL\n", __func__);
		return -ENODEV;
	}
	DBG("mmap called, vma=%p\n", vma);

	ret = videobuf_mmap_mapper(&cam->vb_vidq, vma);

	DBG("vma start=0x%08lx, size=%ld, ret=%d\n",
		(unsigned long)vma->vm_start,
		(unsigned long)vma->vm_end - (unsigned long)vma->vm_start, ret);
	return ret;
}