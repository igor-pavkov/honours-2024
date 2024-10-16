static int vbg_misc_device_user_open(struct inode *inode, struct file *filp)
{
	struct vbg_session *session;
	struct vbg_dev *gdev;

	/* misc_open sets filp->private_data to our misc device */
	gdev = container_of(filp->private_data, struct vbg_dev,
			    misc_device_user);

	session = vbg_core_open_session(gdev, false);
	if (IS_ERR(session))
		return PTR_ERR(session);

	filp->private_data = session;
	return 0;
}