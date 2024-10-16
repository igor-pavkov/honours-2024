static void ShiftMetaOffset(GF_MetaBox *meta, u64 offset)
{
	u32 i, count;
	if (!meta->item_locations) return;

	count = gf_list_count(meta->item_locations->location_entries);
	for (i=0; i<count; i++) {
		GF_ItemLocationEntry *iloc = (GF_ItemLocationEntry *)gf_list_get(meta->item_locations->location_entries, i);
		if (iloc->data_reference_index) continue;
		if (iloc->construction_method == 2) continue;
		if (!iloc->base_offset) {
			GF_ItemExtentEntry *entry = (GF_ItemExtentEntry *)gf_list_get(iloc->extent_entries, 0);
			if (entry && !entry->extent_length && !entry->original_extent_offset && (gf_list_count(iloc->extent_entries)==1) )
				continue;
		}

		iloc->base_offset += offset;
	}
}