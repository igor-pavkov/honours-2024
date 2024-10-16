static int handle_record_type(HexParser *parser)
{
    HexLine *line = &(parser->line);
    switch (line->record_type) {
    case DATA_RECORD:
        parser->current_address =
            (parser->next_address_to_write & NEXT_ADDR_MASK) | line->address;
        /* verify this is a contiguous block of memory */
        if (parser->current_address != parser->next_address_to_write) {
            if (parser->current_rom_index != 0) {
                rom_add_blob_fixed_as(parser->filename, parser->bin_buf,
                                      parser->current_rom_index,
                                      parser->rom_start_address, parser->as);
            }
            parser->rom_start_address = parser->current_address;
            parser->current_rom_index = 0;
        }

        /* copy from line buffer to output bin_buf */
        memcpy(parser->bin_buf + parser->current_rom_index, line->data,
               line->byte_count);
        parser->current_rom_index += line->byte_count;
        parser->total_size += line->byte_count;
        /* save next address to write */
        parser->next_address_to_write =
            parser->current_address + line->byte_count;
        break;

    case EOF_RECORD:
        if (parser->current_rom_index != 0) {
            rom_add_blob_fixed_as(parser->filename, parser->bin_buf,
                                  parser->current_rom_index,
                                  parser->rom_start_address, parser->as);
        }
        return parser->total_size;
    case EXT_SEG_ADDR_RECORD:
    case EXT_LINEAR_ADDR_RECORD:
        if (line->byte_count != 2 && line->address != 0) {
            return -1;
        }

        if (parser->current_rom_index != 0) {
            rom_add_blob_fixed_as(parser->filename, parser->bin_buf,
                                  parser->current_rom_index,
                                  parser->rom_start_address, parser->as);
        }

        /* save next address to write,
         * in case of non-contiguous block of memory */
        parser->next_address_to_write = (line->data[0] << 12) |
                                        (line->data[1] << 4);
        if (line->record_type == EXT_LINEAR_ADDR_RECORD) {
            parser->next_address_to_write <<= 12;
        }

        parser->rom_start_address = parser->next_address_to_write;
        parser->current_rom_index = 0;
        break;

    case START_SEG_ADDR_RECORD:
        if (line->byte_count != 4 && line->address != 0) {
            return -1;
        }

        /* x86 16-bit CS:IP segmented addressing */
        *(parser->start_addr) = (((line->data[0] << 8) | line->data[1]) << 4) +
                                ((line->data[2] << 8) | line->data[3]);
        break;

    case START_LINEAR_ADDR_RECORD:
        if (line->byte_count != 4 && line->address != 0) {
            return -1;
        }

        *(parser->start_addr) = ldl_be_p(line->data);
        break;

    default:
        return -1;
    }

    return parser->total_size;
}