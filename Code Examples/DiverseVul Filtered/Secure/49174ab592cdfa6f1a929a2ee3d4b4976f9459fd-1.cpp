int jp2_validate(jas_stream_t *in)
{
	unsigned char buf[JP2_VALIDATELEN];
#if 0
	jas_stream_t *tmpstream;
	jp2_box_t *box;
#endif

	assert(JAS_STREAM_MAXPUTBACK >= JP2_VALIDATELEN);

	/* Read the validation data (i.e., the data used for detecting
	  the format). */
	if (jas_stream_peek(in, buf, sizeof(buf)) != sizeof(buf))
		return -1;

	/* Is the box type correct? */
	if ((((uint_least32_t)buf[4] << 24) | ((uint_least32_t)buf[5] << 16) | ((uint_least32_t)buf[6] << 8) | (uint_least32_t)buf[7]) !=
	  JP2_BOX_JP)
	{
		return -1;
	}

	return 0;
}