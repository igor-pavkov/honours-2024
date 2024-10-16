static void free_buffer(struct videobuf_queue *vq, struct zr364xx_buffer *buf)
{
	_DBG("%s\n", __func__);

	BUG_ON(in_interrupt());

	videobuf_vmalloc_free(&buf->vb);
	buf->vb.state = VIDEOBUF_NEEDS_INIT;
}