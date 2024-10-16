static void fn_caps_toggle(struct vc_data *vc)
{
	if (rep)
		return;

	chg_vc_kbd_led(kbd, VC_CAPSLOCK);
}