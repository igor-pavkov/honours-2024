void vbg_put_gdev(struct vbg_dev *gdev)
{
	WARN_ON(gdev != vbg_gdev);
	mutex_unlock(&vbg_gdev_mutex);
}