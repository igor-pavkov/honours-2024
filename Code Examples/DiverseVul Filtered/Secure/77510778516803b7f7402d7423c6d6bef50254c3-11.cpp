GF_Err dOps_box_size(GF_Box *s)
{
	GF_OpusSpecificBox *ptr = (GF_OpusSpecificBox *)s;
	ptr->size += 11;
	if (ptr->opcfg.ChannelMappingFamily)
		ptr->size += 2 + ptr->opcfg.OutputChannelCount;

	return GF_OK;
}