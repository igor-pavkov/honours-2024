static int update_clone_start_failure(struct strbuf *err,
				      void *suc_cb,
				      void *idx_task_cb)
{
	struct submodule_update_clone *suc = suc_cb;
	suc->quickstop = 1;
	return 1;
}