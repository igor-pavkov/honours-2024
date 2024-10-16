GF_Err paen_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	FDPartitionEntryBox *ptr = (FDPartitionEntryBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_FPAR:
		BOX_FIELD_ASSIGN(blocks_and_symbols, FilePartitionBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_FECR:
		BOX_FIELD_ASSIGN(FEC_symbol_locations, FECReservoirBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_FIRE:
		BOX_FIELD_ASSIGN(File_symbol_locations, FileReservoirBox)
		return GF_OK;
	}
	return GF_OK;
}