static void free_async(struct async *as)
{
	int i;

	put_pid(as->pid);
	if (as->cred)
		put_cred(as->cred);
	for (i = 0; i < as->urb->num_sgs; i++) {
		if (sg_page(&as->urb->sg[i]))
			kfree(sg_virt(&as->urb->sg[i]));
	}

	kfree(as->urb->sg);
	if (as->usbm == NULL)
		kfree(as->urb->transfer_buffer);
	else
		dec_usb_memory_use_count(as->usbm, &as->usbm->urb_use_count);

	kfree(as->urb->setup_packet);
	usb_free_urb(as->urb);
	usbfs_decrease_memory_usage(as->mem_usage);
	kfree(as);
}