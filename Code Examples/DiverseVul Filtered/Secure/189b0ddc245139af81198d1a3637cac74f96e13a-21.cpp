void free_pipe_info(struct pipe_inode_info *pipe)
{
	unsigned int i;

#ifdef CONFIG_WATCH_QUEUE
	if (pipe->watch_queue)
		watch_queue_clear(pipe->watch_queue);
#endif

	(void) account_pipe_buffers(pipe->user, pipe->nr_accounted, 0);
	free_uid(pipe->user);
	for (i = 0; i < pipe->ring_size; i++) {
		struct pipe_buffer *buf = pipe->bufs + i;
		if (buf->ops)
			pipe_buf_release(pipe, buf);
	}
#ifdef CONFIG_WATCH_QUEUE
	if (pipe->watch_queue)
		put_watch_queue(pipe->watch_queue);
#endif
	if (pipe->tmp_page)
		__free_page(pipe->tmp_page);
	kfree(pipe->bufs);
	kfree(pipe);
}