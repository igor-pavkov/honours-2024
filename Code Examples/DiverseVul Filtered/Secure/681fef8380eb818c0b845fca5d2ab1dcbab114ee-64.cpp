static void destroy_all_async(struct usb_dev_state *ps)
{
	destroy_async(ps, &ps->async_pending);
}