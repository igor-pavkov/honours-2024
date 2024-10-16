void git_treebuilder_free(git_treebuilder *bld)
{
	if (bld == NULL)
		return;

	git_treebuilder_clear(bld);
	git_vector_free(&bld->entries);
	git__free(bld);
}