GF_Err stco_box_size(GF_Box *s)
{
	GF_ChunkOffsetBox *ptr = (GF_ChunkOffsetBox *)s;

	ptr->size += 4 + (4 * ptr->nb_entries);
	return GF_OK;
}