static void stub_free_priv_and_urb(struct stub_priv *priv)
{
	struct urb *urb = priv->urb;

	kfree(urb->setup_packet);
	urb->setup_packet = NULL;

	kfree(urb->transfer_buffer);
	urb->transfer_buffer = NULL;

	list_del(&priv->list);
	kmem_cache_free(stub_priv_cache, priv);
	usb_free_urb(urb);
}