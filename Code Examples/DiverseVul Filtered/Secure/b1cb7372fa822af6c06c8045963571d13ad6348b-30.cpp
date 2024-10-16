static int dvb_frontend_open(struct inode *inode, struct file *file)
{
	struct dvb_device *dvbdev = file->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dvb_adapter *adapter = fe->dvb;
	int ret;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);
	if (fe->exit == DVB_FE_DEVICE_REMOVED)
		return -ENODEV;

	if (adapter->mfe_shared) {
		mutex_lock (&adapter->mfe_lock);

		if (adapter->mfe_dvbdev == NULL)
			adapter->mfe_dvbdev = dvbdev;

		else if (adapter->mfe_dvbdev != dvbdev) {
			struct dvb_device
				*mfedev = adapter->mfe_dvbdev;
			struct dvb_frontend
				*mfe = mfedev->priv;
			struct dvb_frontend_private
				*mfepriv = mfe->frontend_priv;
			int mferetry = (dvb_mfe_wait_time << 1);

			mutex_unlock (&adapter->mfe_lock);
			while (mferetry-- && (mfedev->users != -1 ||
					mfepriv->thread != NULL)) {
				if(msleep_interruptible(500)) {
					if(signal_pending(current))
						return -EINTR;
				}
			}

			mutex_lock (&adapter->mfe_lock);
			if(adapter->mfe_dvbdev != dvbdev) {
				mfedev = adapter->mfe_dvbdev;
				mfe = mfedev->priv;
				mfepriv = mfe->frontend_priv;
				if (mfedev->users != -1 ||
						mfepriv->thread != NULL) {
					mutex_unlock (&adapter->mfe_lock);
					return -EBUSY;
				}
				adapter->mfe_dvbdev = dvbdev;
			}
		}
	}

	if (dvbdev->users == -1 && fe->ops.ts_bus_ctrl) {
		if ((ret = fe->ops.ts_bus_ctrl(fe, 1)) < 0)
			goto err0;

		/* If we took control of the bus, we need to force
		   reinitialization.  This is because many ts_bus_ctrl()
		   functions strobe the RESET pin on the demod, and if the
		   frontend thread already exists then the dvb_init() routine
		   won't get called (which is what usually does initial
		   register configuration). */
		fepriv->reinitialise = 1;
	}

	if ((ret = dvb_generic_open (inode, file)) < 0)
		goto err1;

	if ((file->f_flags & O_ACCMODE) != O_RDONLY) {
		/* normal tune mode when opened R/W */
		fepriv->tune_mode_flags &= ~FE_TUNE_MODE_ONESHOT;
		fepriv->tone = -1;
		fepriv->voltage = -1;

#ifdef CONFIG_MEDIA_CONTROLLER_DVB
		if (fe->dvb->mdev) {
			mutex_lock(&fe->dvb->mdev->graph_mutex);
			if (fe->dvb->mdev->enable_source)
				ret = fe->dvb->mdev->enable_source(
							   dvbdev->entity,
							   &fepriv->pipe);
			mutex_unlock(&fe->dvb->mdev->graph_mutex);
			if (ret) {
				dev_err(fe->dvb->device,
					"Tuner is busy. Error %d\n", ret);
				goto err2;
			}
		}
#endif
		ret = dvb_frontend_start (fe);
		if (ret)
			goto err3;

		/*  empty event queue */
		fepriv->events.eventr = fepriv->events.eventw = 0;
	}

	dvb_frontend_get(fe);

	if (adapter->mfe_shared)
		mutex_unlock (&adapter->mfe_lock);
	return ret;

err3:
#ifdef CONFIG_MEDIA_CONTROLLER_DVB
	if (fe->dvb->mdev) {
		mutex_lock(&fe->dvb->mdev->graph_mutex);
		if (fe->dvb->mdev->disable_source)
			fe->dvb->mdev->disable_source(dvbdev->entity);
		mutex_unlock(&fe->dvb->mdev->graph_mutex);
	}
err2:
#endif
	dvb_generic_release(inode, file);
err1:
	if (dvbdev->users == -1 && fe->ops.ts_bus_ctrl)
		fe->ops.ts_bus_ctrl(fe, 0);
err0:
	if (adapter->mfe_shared)
		mutex_unlock (&adapter->mfe_lock);
	return ret;
}