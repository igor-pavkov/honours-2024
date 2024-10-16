int qemu_input_scale_axis(int value,
                          int min_in, int max_in,
                          int min_out, int max_out)
{
    int64_t range_in = (int64_t)max_in - min_in;
    int64_t range_out = (int64_t)max_out - min_out;

    if (range_in < 1) {
        return min_out + range_out / 2;
    }
    return ((int64_t)value - min_in) * range_out / range_in + min_out;
}