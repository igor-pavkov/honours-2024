static bool parse_record(HexLine *line, uint8_t *our_checksum, const uint8_t c,
                         uint32_t *index, const bool in_process)
{
    /* +-------+---------------+-------+---------------------+--------+
     * | byte  |               |record |                     |        |
     * | count |    address    | type  |        data         |checksum|
     * +-------+---------------+-------+---------------------+--------+
     * ^       ^               ^       ^                     ^        ^
     * |1 byte |    2 bytes    |1 byte |     0-255 bytes     | 1 byte |
     */
    uint8_t value = 0;
    uint32_t idx = *index;
    /* ignore space */
    if (g_ascii_isspace(c)) {
        return true;
    }
    if (!g_ascii_isxdigit(c) || !in_process) {
        return false;
    }
    value = g_ascii_xdigit_value(c);
    value = (idx & 0x1) ? (value & 0xf) : (value << 4);
    if (idx < 2) {
        line->byte_count |= value;
    } else if (2 <= idx && idx < 6) {
        line->address <<= 4;
        line->address += g_ascii_xdigit_value(c);
    } else if (6 <= idx && idx < 8) {
        line->record_type |= value;
    } else if (8 <= idx && idx < 8 + 2 * line->byte_count) {
        line->data[(idx - 8) >> 1] |= value;
    } else if (8 + 2 * line->byte_count <= idx &&
               idx < 10 + 2 * line->byte_count) {
        line->checksum |= value;
    } else {
        return false;
    }
    *our_checksum += value;
    ++(*index);
    return true;
}