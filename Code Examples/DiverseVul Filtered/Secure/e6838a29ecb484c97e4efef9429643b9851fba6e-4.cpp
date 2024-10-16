static bool nfs_request_too_big(struct svc_rqst *rqstp,
				struct svc_procedure *proc)
{
	/*
	 * The ACL code has more careful bounds-checking and is not
	 * susceptible to this problem:
	 */
	if (rqstp->rq_prog != NFS_PROGRAM)
		return false;
	/*
	 * Ditto NFSv4 (which can in theory have argument and reply both
	 * more than a page):
	 */
	if (rqstp->rq_vers >= 4)
		return false;
	/* The reply will be small, we're OK: */
	if (proc->pc_xdrressize > 0 &&
	    proc->pc_xdrressize < XDR_QUADLEN(PAGE_SIZE))
		return false;

	return rqstp->rq_arg.len > PAGE_SIZE;
}