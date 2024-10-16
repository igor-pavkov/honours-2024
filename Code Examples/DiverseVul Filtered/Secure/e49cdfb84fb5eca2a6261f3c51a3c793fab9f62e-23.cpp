static int ok_inflater_decode_length(ok_inflater *inflater, int value) {
    if (value < 8) {
        return value + 3;
    } else {
        int len = OK_INFLATER_LENGTH_TABLE[value];
        unsigned int extra_bits = (unsigned int)((value >> 2) - 1);
        if (extra_bits <= 5) {
            if (!ok_inflater_load_bits(inflater, extra_bits)) {
                return -1;
            }
            len += ok_inflater_read_bits(inflater, extra_bits);
        }
        return len;
    }
}