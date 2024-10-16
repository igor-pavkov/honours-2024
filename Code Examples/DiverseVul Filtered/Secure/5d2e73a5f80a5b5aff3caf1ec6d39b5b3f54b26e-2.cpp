static void buffer_release(struct videobuf_queue *vq,
			   struct videobuf_buffer *vb)
{
	struct zr364xx_buffer *buf = container_of(vb, struct zr364xx_buffer,
						  vb);

	_DBG("%s\n", __func__);
	free_buffer(vq, buf);
}