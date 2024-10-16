static void apply_dependent_coupling(AACContext *ac,
                                     SingleChannelElement *target,
                                     ChannelElement *cce, int index)
{
    IndividualChannelStream *ics = &cce->ch[0].ics;
    const uint16_t *offsets = ics->swb_offset;
    float *dest = target->coeffs;
    const float *src = cce->ch[0].coeffs;
    int g, i, group, k, idx = 0;
    if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP) {
        av_log(ac->avctx, AV_LOG_ERROR,
               "Dependent coupling is not supported together with LTP\n");
        return;
    }
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            if (cce->ch[0].band_type[idx] != ZERO_BT) {
                const float gain = cce->coup.gain[index][idx];
                for (group = 0; group < ics->group_len[g]; group++) {
                    for (k = offsets[i]; k < offsets[i + 1]; k++) {
                        // XXX dsputil-ize
                        dest[group * 128 + k] += gain * src[group * 128 + k];
                    }
                }
            }
        }
        dest += ics->group_len[g] * 128;
        src  += ics->group_len[g] * 128;
    }
}