static void hub_init_func2(struct work_struct *ws)
{
	struct usb_hub *hub = container_of(ws, struct usb_hub, init_work.work);

	hub_activate(hub, HUB_INIT2);
}