int64_t switch_segment(int32_t segment)
{
    location.segment = segment;
    if (segment == NO_SEG) {
        location.offset = absolute.offset;
        in_absolute = true;
    } else {
        location.offset = raa_read(offsets, segment);
        in_absolute = false;
    }
    return location.offset;
}