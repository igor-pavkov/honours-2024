static int co64_required(const MOVTrack *track)
{
    if (track->entry > 0 && track->cluster[track->entry - 1].pos + track->data_offset > UINT32_MAX)
        return 1;
    return 0;
}