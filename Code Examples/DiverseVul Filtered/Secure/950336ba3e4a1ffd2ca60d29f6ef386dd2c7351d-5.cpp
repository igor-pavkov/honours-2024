static void ati_remote2_kill_urbs(struct ati_remote2 *ar2)
{
	usb_kill_urb(ar2->urb[1]);
	usb_kill_urb(ar2->urb[0]);
}