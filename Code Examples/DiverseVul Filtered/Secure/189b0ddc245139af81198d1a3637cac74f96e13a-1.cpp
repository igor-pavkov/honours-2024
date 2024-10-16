static bool anon_pipe_buf_try_steal(struct pipe_inode_info *pipe,
		struct pipe_buffer *buf)
{
	struct page *page = buf->page;

	if (page_count(page) != 1)
		return false;
	memcg_kmem_uncharge_page(page, 0);
	__SetPageLocked(page);
	return true;
}