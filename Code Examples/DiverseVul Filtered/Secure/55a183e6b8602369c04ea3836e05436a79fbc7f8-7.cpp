void BD_OffsetSFTime(GF_BifsDecoder * codec, Double *time)
{
	if ((!codec->is_com_dec && codec->pCurrentProto) || codec->dec_memory_mode) return;
	*time += codec->cts_offset;
}