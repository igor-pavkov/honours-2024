static int airspy_s_frequency(struct file *file, void *priv,
		const struct v4l2_frequency *f)
{
	struct airspy *s = video_drvdata(file);
	int ret;
	u8 buf[4];

	if (f->tuner == 0) {
		s->f_adc = clamp_t(unsigned int, f->frequency,
				bands[0].rangelow,
				bands[0].rangehigh);
		dev_dbg(s->dev, "ADC frequency=%u Hz\n", s->f_adc);
		ret = 0;
	} else if (f->tuner == 1) {
		s->f_rf = clamp_t(unsigned int, f->frequency,
				bands_rf[0].rangelow,
				bands_rf[0].rangehigh);
		dev_dbg(s->dev, "RF frequency=%u Hz\n", s->f_rf);
		buf[0] = (s->f_rf >>  0) & 0xff;
		buf[1] = (s->f_rf >>  8) & 0xff;
		buf[2] = (s->f_rf >> 16) & 0xff;
		buf[3] = (s->f_rf >> 24) & 0xff;
		ret = airspy_ctrl_msg(s, CMD_SET_FREQ, 0, 0, buf, 4);
	} else {
		ret = -EINVAL;
	}

	return ret;
}