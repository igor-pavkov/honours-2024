static int cbs_jpeg_assemble_fragment(CodedBitstreamContext *ctx,
                                       CodedBitstreamFragment *frag)
{
    const CodedBitstreamUnit *unit;
    uint8_t *data;
    size_t size, dp, sp;
    int i;

    size = 4; // SOI + EOI.
    for (i = 0; i < frag->nb_units; i++) {
        unit = &frag->units[i];
        size += 2 + unit->data_size;
        if (unit->type == JPEG_MARKER_SOS) {
            for (sp = 0; sp < unit->data_size; sp++) {
                if (unit->data[sp] == 0xff)
                    ++size;
            }
        }
    }

    frag->data_ref = av_buffer_alloc(size + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!frag->data_ref)
        return AVERROR(ENOMEM);
    data = frag->data_ref->data;

    dp = 0;

    data[dp++] = 0xff;
    data[dp++] = JPEG_MARKER_SOI;

    for (i = 0; i < frag->nb_units; i++) {
        unit = &frag->units[i];

        data[dp++] = 0xff;
        data[dp++] = unit->type;

        if (unit->type != JPEG_MARKER_SOS) {
            memcpy(data + dp, unit->data, unit->data_size);
            dp += unit->data_size;
        } else {
            sp = AV_RB16(unit->data);
            av_assert0(sp <= unit->data_size);
            memcpy(data + dp, unit->data, sp);
            dp += sp;

            for (; sp < unit->data_size; sp++) {
                if (unit->data[sp] == 0xff) {
                    data[dp++] = 0xff;
                    data[dp++] = 0x00;
                } else {
                    data[dp++] = unit->data[sp];
                }
            }
        }
    }

    data[dp++] = 0xff;
    data[dp++] = JPEG_MARKER_EOI;

    av_assert0(dp == size);

    memset(data + size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    frag->data      = data;
    frag->data_size = size;

    return 0;
}