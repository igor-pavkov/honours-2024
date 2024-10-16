static void cpia2_usb_complete(struct urb *urb)
{
	int i;
	unsigned char *cdata;
	static bool frame_ready = false;
	struct camera_data *cam = (struct camera_data *) urb->context;

	if (urb->status!=0) {
		if (!(urb->status == -ENOENT ||
		      urb->status == -ECONNRESET ||
		      urb->status == -ESHUTDOWN))
		{
			DBG("urb->status = %d!\n", urb->status);
		}
		DBG("Stopping streaming\n");
		return;
	}

	if (!cam->streaming || !video_is_registered(&cam->vdev)) {
		LOG("Will now stop the streaming: streaming = %d, present=%d\n",
		    cam->streaming, video_is_registered(&cam->vdev));
		return;
	}

	/***
	 * Packet collater
	 ***/
	//DBG("Collating %d packets\n", urb->number_of_packets);
	for (i = 0; i < urb->number_of_packets; i++) {
		u16 checksum, iso_checksum;
		int j;
		int n = urb->iso_frame_desc[i].actual_length;
		int st = urb->iso_frame_desc[i].status;

		if(cam->workbuff->status == FRAME_READY) {
			struct framebuf *ptr;
			/* Try to find an available buffer */
			DBG("workbuff full, searching\n");
			for (ptr = cam->workbuff->next;
			     ptr != cam->workbuff;
			     ptr = ptr->next)
			{
				if (ptr->status == FRAME_EMPTY) {
					ptr->status = FRAME_READING;
					ptr->length = 0;
					break;
				}
			}
			if (ptr == cam->workbuff)
				break; /* No READING or EMPTY buffers left */

			cam->workbuff = ptr;
		}

		if (cam->workbuff->status == FRAME_EMPTY ||
		    cam->workbuff->status == FRAME_ERROR) {
			cam->workbuff->status = FRAME_READING;
			cam->workbuff->length = 0;
		}

		//DBG("   Packet %d length = %d, status = %d\n", i, n, st);
		cdata = urb->transfer_buffer + urb->iso_frame_desc[i].offset;

		if (st) {
			LOG("cpia2 data error: [%d] len=%d, status = %d\n",
			    i, n, st);
			if(!ALLOW_CORRUPT)
				cam->workbuff->status = FRAME_ERROR;
			continue;
		}

		if(n<=2)
			continue;

		checksum = 0;
		for(j=0; j<n-2; ++j)
			checksum += cdata[j];
		iso_checksum = cdata[j] + cdata[j+1]*256;
		if(checksum != iso_checksum) {
			LOG("checksum mismatch: [%d] len=%d, calculated = %x, checksum = %x\n",
			    i, n, (int)checksum, (int)iso_checksum);
			if(!ALLOW_CORRUPT) {
				cam->workbuff->status = FRAME_ERROR;
				continue;
			}
		}
		n -= 2;

		if(cam->workbuff->status != FRAME_READING) {
			if((0xFF == cdata[0] && 0xD8 == cdata[1]) ||
			   (0xD8 == cdata[0] && 0xFF == cdata[1] &&
			    0 != cdata[2])) {
				/* frame is skipped, but increment total
				 * frame count anyway */
				cam->frame_count++;
			}
			DBG("workbuff not reading, status=%d\n",
			    cam->workbuff->status);
			continue;
		}

		if (cam->frame_size < cam->workbuff->length + n) {
			ERR("buffer overflow! length: %d, n: %d\n",
			    cam->workbuff->length, n);
			cam->workbuff->status = FRAME_ERROR;
			if(cam->workbuff->length > cam->workbuff->max_length)
				cam->workbuff->max_length =
					cam->workbuff->length;
			continue;
		}

		if (cam->workbuff->length == 0) {
			int data_offset;
			if ((0xD8 == cdata[0]) && (0xFF == cdata[1])) {
				data_offset = 1;
			} else if((0xFF == cdata[0]) && (0xD8 == cdata[1])
				  && (0xFF == cdata[2])) {
				data_offset = 2;
			} else {
				DBG("Ignoring packet, not beginning!\n");
				continue;
			}
			DBG("Start of frame pattern found\n");
			cam->workbuff->ts = ktime_get_ns();
			cam->workbuff->seq = cam->frame_count++;
			cam->workbuff->data[0] = 0xFF;
			cam->workbuff->data[1] = 0xD8;
			cam->workbuff->length = 2;
			add_APPn(cam);
			add_COM(cam);
			memcpy(cam->workbuff->data+cam->workbuff->length,
			       cdata+data_offset, n-data_offset);
			cam->workbuff->length += n-data_offset;
		} else if (cam->workbuff->length > 0) {
			memcpy(cam->workbuff->data + cam->workbuff->length,
			       cdata, n);
			cam->workbuff->length += n;
		}

		if ((cam->workbuff->length >= 3) &&
		    (cam->workbuff->data[cam->workbuff->length - 3] == 0xFF) &&
		    (cam->workbuff->data[cam->workbuff->length - 2] == 0xD9) &&
		    (cam->workbuff->data[cam->workbuff->length - 1] == 0xFF)) {
			frame_ready = true;
			cam->workbuff->data[cam->workbuff->length - 1] = 0;
			cam->workbuff->length -= 1;
		} else if ((cam->workbuff->length >= 2) &&
		   (cam->workbuff->data[cam->workbuff->length - 2] == 0xFF) &&
		   (cam->workbuff->data[cam->workbuff->length - 1] == 0xD9)) {
			frame_ready = true;
		}

		if (frame_ready) {
			DBG("Workbuff image size = %d\n",cam->workbuff->length);
			process_frame(cam);

			frame_ready = false;

			if (waitqueue_active(&cam->wq_stream))
				wake_up_interruptible(&cam->wq_stream);
		}
	}

	if(cam->streaming) {
		/* resubmit */
		urb->dev = cam->dev;
		if ((i = usb_submit_urb(urb, GFP_ATOMIC)) != 0)
			ERR("%s: usb_submit_urb ret %d!\n", __func__, i);
	}
}