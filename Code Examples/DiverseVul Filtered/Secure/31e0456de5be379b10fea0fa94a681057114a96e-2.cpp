static int smsusb1_setmode(void *context, int mode)
{
	struct sms_msg_hdr msg = { MSG_SW_RELOAD_REQ, 0, HIF_TASK,
			     sizeof(struct sms_msg_hdr), 0 };

	if (mode < DEVICE_MODE_DVBT || mode > DEVICE_MODE_DVBT_BDA) {
		pr_err("invalid firmware id specified %d\n", mode);
		return -EINVAL;
	}

	return smsusb_sendrequest(context, &msg, sizeof(msg));
}