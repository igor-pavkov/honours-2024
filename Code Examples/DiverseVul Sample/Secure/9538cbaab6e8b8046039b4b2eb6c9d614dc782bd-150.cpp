static int b43_try_request_fw(struct b43_request_fw_context *ctx)
{
	struct b43_wldev *dev = ctx->dev;
	struct b43_firmware *fw = &ctx->dev->fw;
	const u8 rev = ctx->dev->dev->core_rev;
	const char *filename;
	u32 tmshigh;
	int err;

	/* Files for HT and LCN were found by trying one by one */

	/* Get microcode */
	if ((rev >= 5) && (rev <= 10)) {
		filename = "ucode5";
	} else if ((rev >= 11) && (rev <= 12)) {
		filename = "ucode11";
	} else if (rev == 13) {
		filename = "ucode13";
	} else if (rev == 14) {
		filename = "ucode14";
	} else if (rev == 15) {
		filename = "ucode15";
	} else {
		switch (dev->phy.type) {
		case B43_PHYTYPE_N:
			if (rev >= 16)
				filename = "ucode16_mimo";
			else
				goto err_no_ucode;
			break;
		case B43_PHYTYPE_HT:
			if (rev == 29)
				filename = "ucode29_mimo";
			else
				goto err_no_ucode;
			break;
		case B43_PHYTYPE_LCN:
			if (rev == 24)
				filename = "ucode24_mimo";
			else
				goto err_no_ucode;
			break;
		default:
			goto err_no_ucode;
		}
	}
	err = b43_do_request_fw(ctx, filename, &fw->ucode, true);
	if (err)
		goto err_load;

	/* Get PCM code */
	if ((rev >= 5) && (rev <= 10))
		filename = "pcm5";
	else if (rev >= 11)
		filename = NULL;
	else
		goto err_no_pcm;
	fw->pcm_request_failed = false;
	err = b43_do_request_fw(ctx, filename, &fw->pcm, false);
	if (err == -ENOENT) {
		/* We did not find a PCM file? Not fatal, but
		 * core rev <= 10 must do without hwcrypto then. */
		fw->pcm_request_failed = true;
	} else if (err)
		goto err_load;

	/* Get initvals */
	switch (dev->phy.type) {
	case B43_PHYTYPE_A:
		if ((rev >= 5) && (rev <= 10)) {
			tmshigh = ssb_read32(dev->dev->sdev, SSB_TMSHIGH);
			if (tmshigh & B43_TMSHIGH_HAVE_2GHZ_PHY)
				filename = "a0g1initvals5";
			else
				filename = "a0g0initvals5";
		} else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_G:
		if ((rev >= 5) && (rev <= 10))
			filename = "b0g0initvals5";
		else if (rev >= 13)
			filename = "b0g0initvals13";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_N:
		if (rev >= 16)
			filename = "n0initvals16";
		else if ((rev >= 11) && (rev <= 12))
			filename = "n0initvals11";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_LP:
		if (rev == 13)
			filename = "lp0initvals13";
		else if (rev == 14)
			filename = "lp0initvals14";
		else if (rev >= 15)
			filename = "lp0initvals15";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_HT:
		if (rev == 29)
			filename = "ht0initvals29";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_LCN:
		if (rev == 24)
			filename = "lcn0initvals24";
		else
			goto err_no_initvals;
		break;
	default:
		goto err_no_initvals;
	}
	err = b43_do_request_fw(ctx, filename, &fw->initvals, false);
	if (err)
		goto err_load;

	/* Get bandswitch initvals */
	switch (dev->phy.type) {
	case B43_PHYTYPE_A:
		if ((rev >= 5) && (rev <= 10)) {
			tmshigh = ssb_read32(dev->dev->sdev, SSB_TMSHIGH);
			if (tmshigh & B43_TMSHIGH_HAVE_2GHZ_PHY)
				filename = "a0g1bsinitvals5";
			else
				filename = "a0g0bsinitvals5";
		} else if (rev >= 11)
			filename = NULL;
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_G:
		if ((rev >= 5) && (rev <= 10))
			filename = "b0g0bsinitvals5";
		else if (rev >= 11)
			filename = NULL;
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_N:
		if (rev >= 16)
			filename = "n0bsinitvals16";
		else if ((rev >= 11) && (rev <= 12))
			filename = "n0bsinitvals11";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_LP:
		if (rev == 13)
			filename = "lp0bsinitvals13";
		else if (rev == 14)
			filename = "lp0bsinitvals14";
		else if (rev >= 15)
			filename = "lp0bsinitvals15";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_HT:
		if (rev == 29)
			filename = "ht0bsinitvals29";
		else
			goto err_no_initvals;
		break;
	case B43_PHYTYPE_LCN:
		if (rev == 24)
			filename = "lcn0bsinitvals24";
		else
			goto err_no_initvals;
		break;
	default:
		goto err_no_initvals;
	}
	err = b43_do_request_fw(ctx, filename, &fw->initvals_band, false);
	if (err)
		goto err_load;

	fw->opensource = (ctx->req_type == B43_FWTYPE_OPENSOURCE);

	return 0;

err_no_ucode:
	err = ctx->fatal_failure = -EOPNOTSUPP;
	b43err(dev->wl, "The driver does not know which firmware (ucode) "
	       "is required for your device (wl-core rev %u)\n", rev);
	goto error;

err_no_pcm:
	err = ctx->fatal_failure = -EOPNOTSUPP;
	b43err(dev->wl, "The driver does not know which firmware (PCM) "
	       "is required for your device (wl-core rev %u)\n", rev);
	goto error;

err_no_initvals:
	err = ctx->fatal_failure = -EOPNOTSUPP;
	b43err(dev->wl, "The driver does not know which firmware (initvals) "
	       "is required for your device (wl-core rev %u)\n", rev);
	goto error;

err_load:
	/* We failed to load this firmware image. The error message
	 * already is in ctx->errors. Return and let our caller decide
	 * what to do. */
	goto error;

error:
	b43_release_firmware(dev);
	return err;
}