static inline struct kiocb *aio_get_req(struct kioctx *ctx)
{
	struct kiocb *req;
	/* Handle a potential starvation case -- should be exceedingly rare as 
	 * requests will be stuck on fput_head only if the aio_fput_routine is 
	 * delayed and the requests were the last user of the struct file.
	 */
	req = __aio_get_req(ctx);
	if (unlikely(NULL == req)) {
		aio_fput_routine(NULL);
		req = __aio_get_req(ctx);
	}
	return req;
}