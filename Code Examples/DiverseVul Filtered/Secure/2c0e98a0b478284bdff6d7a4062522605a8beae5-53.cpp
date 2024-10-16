static int mov_get_dnxhd_codec_tag(AVFormatContext *s, MOVTrack *track)
{
  int tag = MKTAG('A','V','d','n');
  if (track->par->profile != FF_PROFILE_UNKNOWN &&
      track->par->profile != FF_PROFILE_DNXHD)
      tag = MKTAG('A','V','d','h');
  return tag;
}