jas_image_t *jpc_decode(jas_stream_t *in, const char *optstr)
{
	jpc_dec_importopts_t *opts;
	jpc_dec_t *dec;
	jas_image_t *image;

	dec = 0;
	opts = 0;

	JAS_DBGLOG(100, ("jpc_decode(%p, \"%s\")\n", in, optstr));

	if (!(opts = jpc_dec_opts_create(optstr))) {
		goto error;
	}

	jpc_initluts();

	if (!(dec = jpc_dec_create(opts, in))) {
		goto error;
	}
	jpc_dec_opts_destroy(opts);
	opts = 0;

	/* Do most of the work. */
	if (jpc_dec_decode(dec)) {
		goto error;
	}

	if (jas_image_numcmpts(dec->image) >= 3) {
		jas_image_setclrspc(dec->image, JAS_CLRSPC_SRGB);
		jas_image_setcmpttype(dec->image, 0,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
		jas_image_setcmpttype(dec->image, 1,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
		jas_image_setcmpttype(dec->image, 2,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
	} else {
		jas_image_setclrspc(dec->image, JAS_CLRSPC_SGRAY);
		jas_image_setcmpttype(dec->image, 0,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
	}

	/* Save the return value. */
	image = dec->image;

	/* Stop the image from being discarded. */
	dec->image = 0;

	/* Destroy decoder. */
	jpc_dec_destroy(dec);

	return image;

error:
	if (opts) {
		jpc_dec_opts_destroy(opts);
	}
	if (dec) {
		jpc_dec_destroy(dec);
	}
	return 0;
}