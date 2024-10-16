GF_Err csgp_box_size(GF_Box *s)
{
	u32 i, bits;
	GF_CompactSampleGroupBox *ptr = (GF_CompactSampleGroupBox*)s;
	u32 pattern_size = get_size_by_code( ((ptr->flags>>4) & 0x3) );
	u32 scount_size = get_size_by_code( ((ptr->flags>>2) & 0x3) );
	u32 index_size = get_size_by_code( (ptr->flags & 0x3) );

	ptr->size += 12; //v, flags , grouping_type, pattern_length
	if (ptr->flags & (1<<6))
		ptr->size+=4;

	ptr->size += ptr->pattern_count * (pattern_size + scount_size) / 8;
	bits=0;
	for (i=0; i<ptr->pattern_count; i++)
		bits += ptr->patterns[i].length * index_size;
	ptr->size += bits/8;
	if (bits % 8) ptr->size++;
	return GF_OK;
}