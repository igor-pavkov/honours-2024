static void dm_offload_start(struct dm_offload *o)
{
	blk_start_plug(&o->plug);
	o->cb.callback = flush_current_bio_list;
	list_add(&o->cb.list, &current->plug->cb_list);
}