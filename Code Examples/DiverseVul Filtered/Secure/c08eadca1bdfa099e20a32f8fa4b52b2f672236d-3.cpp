static void request_module_async(struct work_struct *work)
{
	struct em28xx *dev = container_of(work,
			     struct em28xx, request_module_wk);

	/*
	 * The em28xx extensions can be modules or builtin. If the
	 * modules are already loaded or are built in, those extensions
	 * can be initialised right now. Otherwise, the module init
	 * code will do it.
	 */

	/*
	 * Devices with an audio-only intf also have a V4L/DVB/RC
	 * intf. Don't register extensions twice on those devices.
	 */
	if (dev->is_audio_only) {
#if defined(CONFIG_MODULES) && defined(MODULE)
		request_module("em28xx-alsa");
#endif
		return;
	}

	em28xx_init_extension(dev);

#if defined(CONFIG_MODULES) && defined(MODULE)
	if (dev->has_video)
		request_module("em28xx-v4l");
	if (dev->usb_audio_type == EM28XX_USB_AUDIO_CLASS)
		request_module("snd-usb-audio");
	else if (dev->usb_audio_type == EM28XX_USB_AUDIO_VENDOR)
		request_module("em28xx-alsa");
	if (dev->board.has_dvb)
		request_module("em28xx-dvb");
	if (dev->board.buttons ||
	    ((dev->board.ir_codes || dev->board.has_ir_i2c) && !disable_ir))
		request_module("em28xx-rc");
#endif /* CONFIG_MODULES */
}