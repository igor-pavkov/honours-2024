static int is_clcp_track(MOVTrack *track)
{
    return track->tag == MKTAG('c','7','0','8') ||
           track->tag == MKTAG('c','6','0','8');
}