static void zr364xx_fillbuff(struct zr364xx_camera *cam,
			     struct zr364xx_buffer *buf,
			     int jpgsize)
{
	int pos = 0;
	const char *tmpbuf;
	char *vbuf = videobuf_to_vmalloc(&buf->vb);
	unsigned long last_frame;

	if (!vbuf)
		return;

	last_frame = cam->last_frame;
	if (last_frame != -1) {
		tmpbuf = (const char *)cam->buffer.frame[last_frame].lpvbits;
		switch (buf->fmt->fourcc) {
		case V4L2_PIX_FMT_JPEG:
			buf->vb.size = jpgsize;
			memcpy(vbuf, tmpbuf, buf->vb.size);
			break;
		default:
			printk(KERN_DEBUG KBUILD_MODNAME ": unknown format?\n");
		}
		cam->last_frame = -1;
	} else {
		printk(KERN_ERR KBUILD_MODNAME ": =======no frame\n");
		return;
	}
	DBG("%s: Buffer %p size= %d\n", __func__, vbuf, pos);
	/* tell v4l buffer was filled */

	buf->vb.field_count = cam->frame_count * 2;
	buf->vb.ts = ktime_get_ns();
	buf->vb.state = VIDEOBUF_DONE;
}