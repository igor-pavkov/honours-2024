void gf_isom_reset_seq_num(GF_ISOFile *movie)
{
#ifdef GPAC_DISABLE_ISOM_FRAGMENTS
	movie->NextMoofNumber = 0;
#endif
}