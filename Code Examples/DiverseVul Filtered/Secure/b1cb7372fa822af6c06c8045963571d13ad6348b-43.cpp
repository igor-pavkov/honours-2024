int dvb_register_frontend(struct dvb_adapter* dvb,
			  struct dvb_frontend* fe)
{
	struct dvb_frontend_private *fepriv;
	const struct dvb_device dvbdev_template = {
		.users = ~0,
		.writers = 1,
		.readers = (~0)-1,
		.fops = &dvb_frontend_fops,
#if defined(CONFIG_MEDIA_CONTROLLER_DVB)
		.name = fe->ops.info.name,
#endif
		.kernel_ioctl = dvb_frontend_ioctl
	};

	dev_dbg(dvb->device, "%s:\n", __func__);

	if (mutex_lock_interruptible(&frontend_mutex))
		return -ERESTARTSYS;

	fe->frontend_priv = kzalloc(sizeof(struct dvb_frontend_private), GFP_KERNEL);
	if (fe->frontend_priv == NULL) {
		mutex_unlock(&frontend_mutex);
		return -ENOMEM;
	}
	fepriv = fe->frontend_priv;

	kref_init(&fe->refcount);

	/*
	 * After initialization, there need to be two references: one
	 * for dvb_unregister_frontend(), and another one for
	 * dvb_frontend_detach().
	 */
	dvb_frontend_get(fe);

	sema_init(&fepriv->sem, 1);
	init_waitqueue_head (&fepriv->wait_queue);
	init_waitqueue_head (&fepriv->events.wait_queue);
	mutex_init(&fepriv->events.mtx);
	fe->dvb = dvb;
	fepriv->inversion = INVERSION_OFF;

	dev_info(fe->dvb->device,
			"DVB: registering adapter %i frontend %i (%s)...\n",
			fe->dvb->num, fe->id, fe->ops.info.name);

	dvb_register_device (fe->dvb, &fepriv->dvbdev, &dvbdev_template,
			     fe, DVB_DEVICE_FRONTEND, 0);

	/*
	 * Initialize the cache to the proper values according with the
	 * first supported delivery system (ops->delsys[0])
	 */

	fe->dtv_property_cache.delivery_system = fe->ops.delsys[0];
	dvb_frontend_clear_cache(fe);

	mutex_unlock(&frontend_mutex);
	return 0;
}