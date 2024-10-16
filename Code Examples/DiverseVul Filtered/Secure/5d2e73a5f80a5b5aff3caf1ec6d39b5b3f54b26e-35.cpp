static int buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb,
			  enum v4l2_field field)
{
	struct zr364xx_camera *cam = vq->priv_data;
	struct zr364xx_buffer *buf = container_of(vb, struct zr364xx_buffer,
						  vb);
	int rc;

	DBG("%s, field=%d, fmt name = %s\n", __func__, field,
	    cam->fmt ? cam->fmt->name : "");
	if (!cam->fmt)
		return -EINVAL;

	buf->vb.size = cam->width * cam->height * (cam->fmt->depth >> 3);

	if (buf->vb.baddr != 0 && buf->vb.bsize < buf->vb.size) {
		DBG("invalid buffer prepare\n");
		return -EINVAL;
	}

	buf->fmt = cam->fmt;
	buf->vb.width = cam->width;
	buf->vb.height = cam->height;
	buf->vb.field = field;

	if (buf->vb.state == VIDEOBUF_NEEDS_INIT) {
		rc = videobuf_iolock(vq, &buf->vb, NULL);
		if (rc < 0)
			goto fail;
	}

	buf->vb.state = VIDEOBUF_PREPARED;
	return 0;
fail:
	free_buffer(vq, buf);
	return rc;
}