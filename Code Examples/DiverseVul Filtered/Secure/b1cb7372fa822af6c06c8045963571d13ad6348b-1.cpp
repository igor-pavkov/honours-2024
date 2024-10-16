static void dvb_frontend_get_frequency_limits(struct dvb_frontend *fe,
					u32 *freq_min, u32 *freq_max)
{
	*freq_min = max(fe->ops.info.frequency_min, fe->ops.tuner_ops.info.frequency_min);

	if (fe->ops.info.frequency_max == 0)
		*freq_max = fe->ops.tuner_ops.info.frequency_max;
	else if (fe->ops.tuner_ops.info.frequency_max == 0)
		*freq_max = fe->ops.info.frequency_max;
	else
		*freq_max = min(fe->ops.info.frequency_max, fe->ops.tuner_ops.info.frequency_max);

	if (*freq_min == 0 || *freq_max == 0)
		dev_warn(fe->dvb->device, "DVB: adapter %i frontend %u frequency limits undefined - fix the driver\n",
				fe->dvb->num, fe->id);
}