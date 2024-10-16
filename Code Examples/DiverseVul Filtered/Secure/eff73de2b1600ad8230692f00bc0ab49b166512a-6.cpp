static void free_sbufs(struct camera_data *cam)
{
	int i;

	for (i = 0; i < NUM_SBUF; i++) {
		if(cam->sbuf[i].urb) {
			usb_kill_urb(cam->sbuf[i].urb);
			usb_free_urb(cam->sbuf[i].urb);
			cam->sbuf[i].urb = NULL;
		}
		if(cam->sbuf[i].data) {
			kfree(cam->sbuf[i].data);
			cam->sbuf[i].data = NULL;
		}
	}
}