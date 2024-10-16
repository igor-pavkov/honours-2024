static void set_curr_offs(int64_t l_off)
{
        if (in_absolute)
            absolute.offset = l_off;
        else
            offsets = raa_write(offsets, location.segment, l_off);
}