static int parse_hex_blob(const char *filename, hwaddr *addr, uint8_t *hex_blob,
                          size_t hex_blob_size, AddressSpace *as)
{
    bool in_process = false; /* avoid re-enter and
                              * check whether record begin with ':' */
    uint8_t *end = hex_blob + hex_blob_size;
    uint8_t our_checksum = 0;
    uint32_t record_index = 0;
    HexParser parser = {
        .filename = filename,
        .bin_buf = g_malloc(hex_blob_size),
        .start_addr = addr,
        .as = as,
    };

    rom_transaction_begin();

    for (; hex_blob < end; ++hex_blob) {
        switch (*hex_blob) {
        case '\r':
        case '\n':
            if (!in_process) {
                break;
            }

            in_process = false;
            if ((LEN_EXCEPT_DATA + parser.line.byte_count) * 2 !=
                    record_index ||
                our_checksum != 0) {
                parser.total_size = -1;
                goto out;
            }

            if (handle_record_type(&parser) == -1) {
                parser.total_size = -1;
                goto out;
            }
            break;

        /* start of a new record. */
        case ':':
            memset(&parser.line, 0, sizeof(HexLine));
            in_process = true;
            record_index = 0;
            break;

        /* decoding lines */
        default:
            if (!parse_record(&parser.line, &our_checksum, *hex_blob,
                              &record_index, in_process)) {
                parser.total_size = -1;
                goto out;
            }
            break;
        }
    }

out:
    g_free(parser.bin_buf);
    rom_transaction_end(parser.total_size != -1);
    return parser.total_size;
}