static void lsr_read_duration(GF_LASeRCodec *lsr, GF_Node *n)
{
	lsr_read_duration_ex(lsr, n, TAG_SVG_ATT_dur, NULL, "dur", GF_TRUE);
}