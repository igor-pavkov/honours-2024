static int submit_urbs(struct camera_data *cam)
{
	struct urb *urb;
	int fx, err, i, j;

	for(i=0; i<NUM_SBUF; ++i) {
		if (cam->sbuf[i].data)
			continue;
		cam->sbuf[i].data =
		    kmalloc_array(FRAME_SIZE_PER_DESC, FRAMES_PER_DESC,
				  GFP_KERNEL);
		if (!cam->sbuf[i].data) {
			while (--i >= 0) {
				kfree(cam->sbuf[i].data);
				cam->sbuf[i].data = NULL;
			}
			return -ENOMEM;
		}
	}

	/* We double buffer the Isoc lists, and also know the polling
	 * interval is every frame (1 == (1 << (bInterval -1))).
	 */
	for(i=0; i<NUM_SBUF; ++i) {
		if(cam->sbuf[i].urb) {
			continue;
		}
		urb = usb_alloc_urb(FRAMES_PER_DESC, GFP_KERNEL);
		if (!urb) {
			for (j = 0; j < i; j++)
				usb_free_urb(cam->sbuf[j].urb);
			return -ENOMEM;
		}

		cam->sbuf[i].urb = urb;
		urb->dev = cam->dev;
		urb->context = cam;
		urb->pipe = usb_rcvisocpipe(cam->dev, 1 /*ISOC endpoint*/);
		urb->transfer_flags = URB_ISO_ASAP;
		urb->transfer_buffer = cam->sbuf[i].data;
		urb->complete = cpia2_usb_complete;
		urb->number_of_packets = FRAMES_PER_DESC;
		urb->interval = 1;
		urb->transfer_buffer_length =
			FRAME_SIZE_PER_DESC * FRAMES_PER_DESC;

		for (fx = 0; fx < FRAMES_PER_DESC; fx++) {
			urb->iso_frame_desc[fx].offset =
				FRAME_SIZE_PER_DESC * fx;
			urb->iso_frame_desc[fx].length = FRAME_SIZE_PER_DESC;
		}
	}


	/* Queue the ISO urbs, and resubmit in the completion handler */
	for(i=0; i<NUM_SBUF; ++i) {
		err = usb_submit_urb(cam->sbuf[i].urb, GFP_KERNEL);
		if (err) {
			ERR("usb_submit_urb[%d]() = %d\n", i, err);
			return err;
		}
	}

	return 0;
}