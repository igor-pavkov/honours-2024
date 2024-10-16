static unsigned int airspy_convert_stream(struct airspy *s,
		void *dst, void *src, unsigned int src_len)
{
	unsigned int dst_len;

	if (s->pixelformat == V4L2_SDR_FMT_RU12LE) {
		memcpy(dst, src, src_len);
		dst_len = src_len;
	} else {
		dst_len = 0;
	}

	/* calculate sample rate and output it in 10 seconds intervals */
	if (unlikely(time_is_before_jiffies(s->jiffies_next))) {
		#define MSECS 10000UL
		unsigned int msecs = jiffies_to_msecs(jiffies -
				s->jiffies_next + msecs_to_jiffies(MSECS));
		unsigned int samples = s->sample - s->sample_measured;

		s->jiffies_next = jiffies + msecs_to_jiffies(MSECS);
		s->sample_measured = s->sample;
		dev_dbg(s->dev, "slen=%u samples=%u msecs=%u sample rate=%lu\n",
				src_len, samples, msecs,
				samples * 1000UL / msecs);
	}

	/* total number of samples */
	s->sample += src_len / 2;

	return dst_len;
}