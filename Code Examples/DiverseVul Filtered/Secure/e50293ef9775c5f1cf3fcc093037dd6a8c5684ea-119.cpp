static int hub_post_reset(struct usb_interface *intf)
{
	struct usb_hub *hub = usb_get_intfdata(intf);

	hub->in_reset = 0;
	hub_pm_barrier_for_all_ports(hub);
	hub_activate(hub, HUB_POST_RESET);
	return 0;
}