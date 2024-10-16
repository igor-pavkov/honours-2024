static int dvb_frontend_ioctl(struct file *file, unsigned int cmd, void *parg)
{
	struct dvb_device *dvbdev = file->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	int err;

	dev_dbg(fe->dvb->device, "%s: (%d)\n", __func__, _IOC_NR(cmd));
	if (down_interruptible(&fepriv->sem))
		return -ERESTARTSYS;

	if (fe->exit != DVB_FE_NO_EXIT) {
		up(&fepriv->sem);
		return -ENODEV;
	}

	/*
	 * If the frontend is opened in read-only mode, only the ioctls
	 * that don't interfere with the tune logic should be accepted.
	 * That allows an external application to monitor the DVB QoS and
	 * statistics parameters.
	 *
	 * That matches all _IOR() ioctls, except for two special cases:
	 *   - FE_GET_EVENT is part of the tuning logic on a DVB application;
	 *   - FE_DISEQC_RECV_SLAVE_REPLY is part of DiSEqC 2.0
	 *     setup
	 * So, those two ioctls should also return -EPERM, as otherwise
	 * reading from them would interfere with a DVB tune application
	 */
	if ((file->f_flags & O_ACCMODE) == O_RDONLY
	    && (_IOC_DIR(cmd) != _IOC_READ
		|| cmd == FE_GET_EVENT
		|| cmd == FE_DISEQC_RECV_SLAVE_REPLY)) {
		up(&fepriv->sem);
		return -EPERM;
	}

	err = dvb_frontend_handle_ioctl(file, cmd, parg);

	up(&fepriv->sem);
	return err;
}