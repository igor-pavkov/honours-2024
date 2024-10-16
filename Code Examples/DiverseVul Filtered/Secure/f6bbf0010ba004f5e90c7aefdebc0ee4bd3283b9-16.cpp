static void vhost_vdpa_remove(struct vdpa_device *vdpa)
{
	struct vhost_vdpa *v = vdpa_get_drvdata(vdpa);
	int opened;

	cdev_device_del(&v->cdev, &v->dev);

	do {
		opened = atomic_cmpxchg(&v->opened, 0, 1);
		if (!opened)
			break;
		wait_for_completion(&v->completion);
	} while (1);

	put_device(&v->dev);
}