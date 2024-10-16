static void buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
	struct zr364xx_buffer *buf = container_of(vb, struct zr364xx_buffer,
						  vb);
	struct zr364xx_camera *cam = vq->priv_data;

	_DBG("%s\n", __func__);

	buf->vb.state = VIDEOBUF_QUEUED;
	list_add_tail(&buf->vb.queue, &cam->vidq.active);
}