static int dvb_frontend_release(struct inode *inode, struct file *file)
{
	struct dvb_device *dvbdev = file->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	int ret;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	if ((file->f_flags & O_ACCMODE) != O_RDONLY) {
		fepriv->release_jiffies = jiffies;
		mb();
	}

	ret = dvb_generic_release (inode, file);

	if (dvbdev->users == -1) {
		wake_up(&fepriv->wait_queue);
#ifdef CONFIG_MEDIA_CONTROLLER_DVB
		if (fe->dvb->mdev) {
			mutex_lock(&fe->dvb->mdev->graph_mutex);
			if (fe->dvb->mdev->disable_source)
				fe->dvb->mdev->disable_source(dvbdev->entity);
			mutex_unlock(&fe->dvb->mdev->graph_mutex);
		}
#endif
		if (fe->exit != DVB_FE_NO_EXIT)
			wake_up(&dvbdev->wait_queue);
		if (fe->ops.ts_bus_ctrl)
			fe->ops.ts_bus_ctrl(fe, 0);
	}

	dvb_frontend_put(fe);

	return ret;
}