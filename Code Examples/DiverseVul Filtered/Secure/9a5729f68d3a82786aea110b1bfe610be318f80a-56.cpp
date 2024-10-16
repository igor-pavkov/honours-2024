int sisusb_reset_text_mode(struct sisusb_usb_data *sisusb, int init)
{
	int ret = 0, slot = sisusb->font_slot, i;
	const struct font_desc *myfont;
	u8 *tempbuf;
	u16 *tempbufb;
	static const char bootstring[] =
		"SiSUSB VGA text console, (C) 2005 Thomas Winischhofer.";
	static const char bootlogo[] = "(o_ //\\ V_/_";

	/* sisusb->lock is down */

	if (!sisusb->SiS_Pr)
		return 1;

	sisusb->SiS_Pr->IOAddress = SISUSB_PCI_IOPORTBASE + 0x30;
	sisusb->SiS_Pr->sisusb = (void *)sisusb;

	/* Set mode 0x03 */
	SiSUSBSetMode(sisusb->SiS_Pr, 0x03);

	myfont = find_font("VGA8x16");
	if (!myfont)
		return 1;

	tempbuf = vmalloc(8192);
	if (!tempbuf)
		return 1;

	for (i = 0; i < 256; i++)
		memcpy(tempbuf + (i * 32), myfont->data + (i * 16), 16);

	/* Upload default font */
	ret = sisusbcon_do_font_op(sisusb, 1, 0, tempbuf, 8192,
			0, 1, NULL, 16, 0);

	vfree(tempbuf);

	/* Upload user font (and reset current slot) */
	if (sisusb->font_backup) {
		ret |= sisusbcon_do_font_op(sisusb, 1, 2, sisusb->font_backup,
				8192, sisusb->font_backup_512, 1, NULL,
				sisusb->font_backup_height, 0);
		if (slot != 2)
			sisusbcon_do_font_op(sisusb, 1, 0, NULL, 0, 0, 1,
					NULL, 16, 0);
	}

	if (init && !sisusb->scrbuf) {

		tempbuf = vmalloc(8192);
		if (tempbuf) {

			i = 4096;
			tempbufb = (u16 *)tempbuf;
			while (i--)
				*(tempbufb++) = 0x0720;

			i = 0;
			tempbufb = (u16 *)tempbuf;
			while (bootlogo[i]) {
				*(tempbufb++) = 0x0700 | bootlogo[i++];
				if (!(i % 4))
					tempbufb += 76;
			}

			i = 0;
			tempbufb = (u16 *)tempbuf + 6;
			while (bootstring[i])
				*(tempbufb++) = 0x0700 | bootstring[i++];

			ret |= sisusb_copy_memory(sisusb, tempbuf,
					sisusb->vrambase, 8192);

			vfree(tempbuf);

		}

	} else if (sisusb->scrbuf) {
		ret |= sisusb_copy_memory(sisusb, (char *)sisusb->scrbuf,
				sisusb->vrambase, sisusb->scrbuf_size);
	}

	if (sisusb->sisusb_cursor_size_from >= 0 &&
			sisusb->sisusb_cursor_size_to >= 0) {
		sisusb_setidxreg(sisusb, SISCR, 0x0a,
				sisusb->sisusb_cursor_size_from);
		sisusb_setidxregandor(sisusb, SISCR, 0x0b, 0xe0,
				sisusb->sisusb_cursor_size_to);
	} else {
		sisusb_setidxreg(sisusb, SISCR, 0x0a, 0x2d);
		sisusb_setidxreg(sisusb, SISCR, 0x0b, 0x0e);
		sisusb->sisusb_cursor_size_to = -1;
	}

	slot = sisusb->sisusb_cursor_loc;
	if (slot < 0)
		slot = 0;

	sisusb->sisusb_cursor_loc = -1;
	sisusb->bad_cursor_pos = 1;

	sisusb_set_cursor(sisusb, slot);

	sisusb_setidxreg(sisusb, SISCR, 0x0c, (sisusb->cur_start_addr >> 8));
	sisusb_setidxreg(sisusb, SISCR, 0x0d, (sisusb->cur_start_addr & 0xff));

	sisusb->textmodedestroyed = 0;

	/* sisusb->lock is down */

	return ret;
}