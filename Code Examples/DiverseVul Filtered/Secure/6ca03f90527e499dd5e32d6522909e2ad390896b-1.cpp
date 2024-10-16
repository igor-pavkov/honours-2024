static void fn_caps_on(struct vc_data *vc)
{
	if (rep)
		return;

	set_vc_kbd_led(kbd, VC_CAPSLOCK);
}