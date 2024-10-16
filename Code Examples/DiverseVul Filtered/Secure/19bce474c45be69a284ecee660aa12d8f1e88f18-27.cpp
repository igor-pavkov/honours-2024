static void __build_feature_ctl(struct usb_mixer_interface *mixer,
				const struct usbmix_name_map *imap,
				unsigned int ctl_mask, int control,
				struct usb_audio_term *iterm,
				struct usb_audio_term *oterm,
				int unitid, int nameid, int readonly_mask)
{
	struct usb_feature_control_info *ctl_info;
	unsigned int len = 0;
	int mapped_name = 0;
	struct snd_kcontrol *kctl;
	struct usb_mixer_elem_info *cval;
	const struct usbmix_name_map *map;
	unsigned int range;

	if (control == UAC_FU_GRAPHIC_EQUALIZER) {
		/* FIXME: not supported yet */
		return;
	}

	map = find_map(imap, unitid, control);
	if (check_ignored_ctl(map))
		return;

	cval = kzalloc(sizeof(*cval), GFP_KERNEL);
	if (!cval)
		return;
	snd_usb_mixer_elem_init_std(&cval->head, mixer, unitid);
	cval->control = control;
	cval->cmask = ctl_mask;

	ctl_info = get_feature_control_info(control);
	if (!ctl_info) {
		kfree(cval);
		return;
	}
	if (mixer->protocol == UAC_VERSION_1)
		cval->val_type = ctl_info->type;
	else /* UAC_VERSION_2 */
		cval->val_type = ctl_info->type_uac2 >= 0 ?
			ctl_info->type_uac2 : ctl_info->type;

	if (ctl_mask == 0) {
		cval->channels = 1;	/* master channel */
		cval->master_readonly = readonly_mask;
	} else {
		int i, c = 0;
		for (i = 0; i < 16; i++)
			if (ctl_mask & (1 << i))
				c++;
		cval->channels = c;
		cval->ch_readonly = readonly_mask;
	}

	/*
	 * If all channels in the mask are marked read-only, make the control
	 * read-only. snd_usb_set_cur_mix_value() will check the mask again and won't
	 * issue write commands to read-only channels.
	 */
	if (cval->channels == readonly_mask)
		kctl = snd_ctl_new1(&usb_feature_unit_ctl_ro, cval);
	else
		kctl = snd_ctl_new1(&usb_feature_unit_ctl, cval);

	if (!kctl) {
		usb_audio_err(mixer->chip, "cannot malloc kcontrol\n");
		kfree(cval);
		return;
	}
	kctl->private_free = snd_usb_mixer_elem_free;

	len = check_mapped_name(map, kctl->id.name, sizeof(kctl->id.name));
	mapped_name = len != 0;
	if (!len && nameid)
		len = snd_usb_copy_string_desc(mixer->chip, nameid,
				kctl->id.name, sizeof(kctl->id.name));

	switch (control) {
	case UAC_FU_MUTE:
	case UAC_FU_VOLUME:
		/*
		 * determine the control name.  the rule is:
		 * - if a name id is given in descriptor, use it.
		 * - if the connected input can be determined, then use the name
		 *   of terminal type.
		 * - if the connected output can be determined, use it.
		 * - otherwise, anonymous name.
		 */
		if (!len) {
			if (iterm)
				len = get_term_name(mixer->chip, iterm,
						    kctl->id.name,
						    sizeof(kctl->id.name), 1);
			if (!len && oterm)
				len = get_term_name(mixer->chip, oterm,
						    kctl->id.name,
						    sizeof(kctl->id.name), 1);
			if (!len)
				snprintf(kctl->id.name, sizeof(kctl->id.name),
					 "Feature %d", unitid);
		}

		if (!mapped_name)
			check_no_speaker_on_headset(kctl, mixer->chip->card);

		/*
		 * determine the stream direction:
		 * if the connected output is USB stream, then it's likely a
		 * capture stream.  otherwise it should be playback (hopefully :)
		 */
		if (!mapped_name && oterm && !(oterm->type >> 16)) {
			if ((oterm->type & 0xff00) == 0x0100)
				append_ctl_name(kctl, " Capture");
			else
				append_ctl_name(kctl, " Playback");
		}
		append_ctl_name(kctl, control == UAC_FU_MUTE ?
				" Switch" : " Volume");
		break;
	default:
		if (!len)
			strlcpy(kctl->id.name, audio_feature_info[control-1].name,
				sizeof(kctl->id.name));
		break;
	}

	/* get min/max values */
	get_min_max_with_quirks(cval, 0, kctl);

	if (control == UAC_FU_VOLUME) {
		check_mapped_dB(map, cval);
		if (cval->dBmin < cval->dBmax || !cval->initialized) {
			kctl->tlv.c = snd_usb_mixer_vol_tlv;
			kctl->vd[0].access |=
				SNDRV_CTL_ELEM_ACCESS_TLV_READ |
				SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK;
		}
	}

	snd_usb_mixer_fu_apply_quirk(mixer, cval, unitid, kctl);

	range = (cval->max - cval->min) / cval->res;
	/*
	 * Are there devices with volume range more than 255? I use a bit more
	 * to be sure. 384 is a resolution magic number found on Logitech
	 * devices. It will definitively catch all buggy Logitech devices.
	 */
	if (range > 384) {
		usb_audio_warn(mixer->chip,
			       "Warning! Unlikely big volume range (=%u), cval->res is probably wrong.",
			       range);
		usb_audio_warn(mixer->chip,
			       "[%d] FU [%s] ch = %d, val = %d/%d/%d",
			       cval->head.id, kctl->id.name, cval->channels,
			       cval->min, cval->max, cval->res);
	}

	usb_audio_dbg(mixer->chip, "[%d] FU [%s] ch = %d, val = %d/%d/%d\n",
		      cval->head.id, kctl->id.name, cval->channels,
		      cval->min, cval->max, cval->res);
	snd_usb_mixer_add_control(&cval->head, kctl);
}