static void dm_offload_end(struct dm_offload *o)
{
	list_del(&o->cb.list);
	blk_finish_plug(&o->plug);
}