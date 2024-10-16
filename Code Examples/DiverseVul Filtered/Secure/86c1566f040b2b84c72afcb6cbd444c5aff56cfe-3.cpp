Bool CheckHintFormat(GF_TrackBox *trak, u32 HintType)
{
	if (!IsHintTrack(trak)) return GF_FALSE;
	if (GetHintFormat(trak) != HintType) return GF_FALSE;
	return GF_TRUE;
}