unsigned int git_treebuilder_entrycount(git_treebuilder *bld)
{
	assert(bld);
	return (unsigned int)bld->entrycount;
}