static void fn_num(struct vc_data *vc)
{
	if (vc_kbd_mode(kbd, VC_APPLIC))
		applkey(vc, 'P', 1);
	else
		fn_bare_num(vc);
}