static int dvb_frontend_handle_ioctl(struct file *file,
				     unsigned int cmd, void *parg)
{
	struct dvb_device *dvbdev = file->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int i, err = -ENOTSUPP;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	switch (cmd) {
	case FE_SET_PROPERTY: {
		struct dtv_properties *tvps = parg;
		struct dtv_property *tvp = NULL;

		dev_dbg(fe->dvb->device, "%s: properties.num = %d\n",
			__func__, tvps->num);
		dev_dbg(fe->dvb->device, "%s: properties.props = %p\n",
			__func__, tvps->props);

		/*
		 * Put an arbitrary limit on the number of messages that can
		 * be sent at once
		 */
		if (!tvps->num || (tvps->num > DTV_IOCTL_MAX_MSGS))
			return -EINVAL;

		tvp = memdup_user(tvps->props, tvps->num * sizeof(*tvp));
		if (IS_ERR(tvp))
			return PTR_ERR(tvp);

		for (i = 0; i < tvps->num; i++) {
			err = dtv_property_process_set(fe, file,
							(tvp + i)->cmd,
							(tvp + i)->u.data);
			if (err < 0) {
				kfree(tvp);
				return err;
			}
		}
		kfree(tvp);
		err = 0;
		break;
	}
	case FE_GET_PROPERTY: {
		struct dtv_properties *tvps = parg;
		struct dtv_property *tvp = NULL;
		struct dtv_frontend_properties getp = fe->dtv_property_cache;

		dev_dbg(fe->dvb->device, "%s: properties.num = %d\n",
			__func__, tvps->num);
		dev_dbg(fe->dvb->device, "%s: properties.props = %p\n",
			__func__, tvps->props);

		/*
		 * Put an arbitrary limit on the number of messages that can
		 * be sent at once
		 */
		if (!tvps->num || (tvps->num > DTV_IOCTL_MAX_MSGS))
			return -EINVAL;

		tvp = memdup_user(tvps->props, tvps->num * sizeof(*tvp));
		if (IS_ERR(tvp))
			return PTR_ERR(tvp);

		/*
		 * Let's use our own copy of property cache, in order to
		 * avoid mangling with DTV zigzag logic, as drivers might
		 * return crap, if they don't check if the data is available
		 * before updating the properties cache.
		 */
		if (fepriv->state != FESTATE_IDLE) {
			err = dtv_get_frontend(fe, &getp, NULL);
			if (err < 0) {
				kfree(tvp);
				return err;
			}
		}
		for (i = 0; i < tvps->num; i++) {
			err = dtv_property_process_get(fe, &getp,
						       tvp + i, file);
			if (err < 0) {
				kfree(tvp);
				return err;
			}
		}

		if (copy_to_user((void __user *)tvps->props, tvp,
				 tvps->num * sizeof(struct dtv_property))) {
			kfree(tvp);
			return -EFAULT;
		}
		kfree(tvp);
		err = 0;
		break;
	}

	case FE_GET_INFO: {
		struct dvb_frontend_info* info = parg;

		memcpy(info, &fe->ops.info, sizeof(struct dvb_frontend_info));
		dvb_frontend_get_frequency_limits(fe, &info->frequency_min, &info->frequency_max);

		/*
		 * Associate the 4 delivery systems supported by DVBv3
		 * API with their DVBv5 counterpart. For the other standards,
		 * use the closest type, assuming that it would hopefully
		 * work with a DVBv3 application.
		 * It should be noticed that, on multi-frontend devices with
		 * different types (terrestrial and cable, for example),
		 * a pure DVBv3 application won't be able to use all delivery
		 * systems. Yet, changing the DVBv5 cache to the other delivery
		 * system should be enough for making it work.
		 */
		switch (dvbv3_type(c->delivery_system)) {
		case DVBV3_QPSK:
			info->type = FE_QPSK;
			break;
		case DVBV3_ATSC:
			info->type = FE_ATSC;
			break;
		case DVBV3_QAM:
			info->type = FE_QAM;
			break;
		case DVBV3_OFDM:
			info->type = FE_OFDM;
			break;
		default:
			dev_err(fe->dvb->device,
					"%s: doesn't know how to handle a DVBv3 call to delivery system %i\n",
					__func__, c->delivery_system);
			fe->ops.info.type = FE_OFDM;
		}
		dev_dbg(fe->dvb->device, "%s: current delivery system on cache: %d, V3 type: %d\n",
				 __func__, c->delivery_system, fe->ops.info.type);

		/* Set CAN_INVERSION_AUTO bit on in other than oneshot mode */
		if (!(fepriv->tune_mode_flags & FE_TUNE_MODE_ONESHOT))
			info->caps |= FE_CAN_INVERSION_AUTO;
		err = 0;
		break;
	}

	case FE_READ_STATUS: {
		enum fe_status *status = parg;

		/* if retune was requested but hasn't occurred yet, prevent
		 * that user get signal state from previous tuning */
		if (fepriv->state == FESTATE_RETUNE ||
		    fepriv->state == FESTATE_ERROR) {
			err=0;
			*status = 0;
			break;
		}

		if (fe->ops.read_status)
			err = fe->ops.read_status(fe, status);
		break;
	}

	case FE_DISEQC_RESET_OVERLOAD:
		if (fe->ops.diseqc_reset_overload) {
			err = fe->ops.diseqc_reset_overload(fe);
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		}
		break;

	case FE_DISEQC_SEND_MASTER_CMD:
		if (fe->ops.diseqc_send_master_cmd) {
			struct dvb_diseqc_master_cmd *cmd = parg;

			if (cmd->msg_len > sizeof(cmd->msg)) {
				err = -EINVAL;
				break;
			}
			err = fe->ops.diseqc_send_master_cmd(fe, cmd);
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		}
		break;

	case FE_DISEQC_SEND_BURST:
		if (fe->ops.diseqc_send_burst) {
			err = fe->ops.diseqc_send_burst(fe,
						(enum fe_sec_mini_cmd)parg);
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		}
		break;

	case FE_SET_TONE:
		if (fe->ops.set_tone) {
			err = fe->ops.set_tone(fe,
					       (enum fe_sec_tone_mode)parg);
			fepriv->tone = (enum fe_sec_tone_mode)parg;
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		}
		break;

	case FE_SET_VOLTAGE:
		if (fe->ops.set_voltage) {
			err = fe->ops.set_voltage(fe,
						  (enum fe_sec_voltage)parg);
			fepriv->voltage = (enum fe_sec_voltage)parg;
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		}
		break;

	case FE_DISEQC_RECV_SLAVE_REPLY:
		if (fe->ops.diseqc_recv_slave_reply)
			err = fe->ops.diseqc_recv_slave_reply(fe, parg);
		break;

	case FE_ENABLE_HIGH_LNB_VOLTAGE:
		if (fe->ops.enable_high_lnb_voltage)
			err = fe->ops.enable_high_lnb_voltage(fe, (long) parg);
		break;

	case FE_SET_FRONTEND_TUNE_MODE:
		fepriv->tune_mode_flags = (unsigned long) parg;
		err = 0;
		break;

	/* DEPRECATED dish control ioctls */

	case FE_DISHNETWORK_SEND_LEGACY_CMD:
		if (fe->ops.dishnetwork_send_legacy_command) {
			err = fe->ops.dishnetwork_send_legacy_command(fe,
							 (unsigned long)parg);
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		} else if (fe->ops.set_voltage) {
			/*
			 * NOTE: This is a fallback condition.  Some frontends
			 * (stv0299 for instance) take longer than 8msec to
			 * respond to a set_voltage command.  Those switches
			 * need custom routines to switch properly.  For all
			 * other frontends, the following should work ok.
			 * Dish network legacy switches (as used by Dish500)
			 * are controlled by sending 9-bit command words
			 * spaced 8msec apart.
			 * the actual command word is switch/port dependent
			 * so it is up to the userspace application to send
			 * the right command.
			 * The command must always start with a '0' after
			 * initialization, so parg is 8 bits and does not
			 * include the initialization or start bit
			 */
			unsigned long swcmd = ((unsigned long) parg) << 1;
			ktime_t nexttime;
			ktime_t tv[10];
			int i;
			u8 last = 1;
			if (dvb_frontend_debug)
				dprintk("%s switch command: 0x%04lx\n",
					__func__, swcmd);
			nexttime = ktime_get_boottime();
			if (dvb_frontend_debug)
				tv[0] = nexttime;
			/* before sending a command, initialize by sending
			 * a 32ms 18V to the switch
			 */
			fe->ops.set_voltage(fe, SEC_VOLTAGE_18);
			dvb_frontend_sleep_until(&nexttime, 32000);

			for (i = 0; i < 9; i++) {
				if (dvb_frontend_debug)
					tv[i+1] = ktime_get_boottime();
				if ((swcmd & 0x01) != last) {
					/* set voltage to (last ? 13V : 18V) */
					fe->ops.set_voltage(fe, (last) ? SEC_VOLTAGE_13 : SEC_VOLTAGE_18);
					last = (last) ? 0 : 1;
				}
				swcmd = swcmd >> 1;
				if (i != 8)
					dvb_frontend_sleep_until(&nexttime, 8000);
			}
			if (dvb_frontend_debug) {
				dprintk("%s(%d): switch delay (should be 32k followed by all 8k)\n",
					__func__, fe->dvb->num);
				for (i = 1; i < 10; i++)
					pr_info("%d: %d\n", i,
					(int) ktime_us_delta(tv[i], tv[i-1]));
			}
			err = 0;
			fepriv->state = FESTATE_DISEQC;
			fepriv->status = 0;
		}
		break;

	/* DEPRECATED statistics ioctls */

	case FE_READ_BER:
		if (fe->ops.read_ber) {
			if (fepriv->thread)
				err = fe->ops.read_ber(fe, parg);
			else
				err = -EAGAIN;
		}
		break;

	case FE_READ_SIGNAL_STRENGTH:
		if (fe->ops.read_signal_strength) {
			if (fepriv->thread)
				err = fe->ops.read_signal_strength(fe, parg);
			else
				err = -EAGAIN;
		}
		break;

	case FE_READ_SNR:
		if (fe->ops.read_snr) {
			if (fepriv->thread)
				err = fe->ops.read_snr(fe, parg);
			else
				err = -EAGAIN;
		}
		break;

	case FE_READ_UNCORRECTED_BLOCKS:
		if (fe->ops.read_ucblocks) {
			if (fepriv->thread)
				err = fe->ops.read_ucblocks(fe, parg);
			else
				err = -EAGAIN;
		}
		break;

	/* DEPRECATED DVBv3 ioctls */

	case FE_SET_FRONTEND:
		err = dvbv3_set_delivery_system(fe);
		if (err)
			break;

		err = dtv_property_cache_sync(fe, c, parg);
		if (err)
			break;
		err = dtv_set_frontend(fe);
		break;
	case FE_GET_EVENT:
		err = dvb_frontend_get_event (fe, parg, file->f_flags);
		break;

	case FE_GET_FRONTEND: {
		struct dtv_frontend_properties getp = fe->dtv_property_cache;

		/*
		 * Let's use our own copy of property cache, in order to
		 * avoid mangling with DTV zigzag logic, as drivers might
		 * return crap, if they don't check if the data is available
		 * before updating the properties cache.
		 */
		err = dtv_get_frontend(fe, &getp, parg);
		break;
	}

	default:
		return -ENOTSUPP;
	} /* switch */

	return err;
}