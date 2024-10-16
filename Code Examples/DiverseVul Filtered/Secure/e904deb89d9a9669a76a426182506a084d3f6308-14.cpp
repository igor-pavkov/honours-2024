void gitmodules_config_oid(const struct object_id *commit_oid)
{
	struct strbuf rev = STRBUF_INIT;
	struct object_id oid;

	submodule_cache_check_init(the_repository);

	if (gitmodule_oid_from_commit(commit_oid, &oid, &rev)) {
		git_config_from_blob_oid(gitmodules_cb, rev.buf,
					 &oid, the_repository);
	}
	strbuf_release(&rev);

	the_repository->submodule_cache->gitmodules_read = 1;
}