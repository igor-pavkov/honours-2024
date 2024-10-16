static void spectral_to_sample(AACContext *ac)
{
    int i, type;
    void (*imdct_and_window)(AACContext *ac, SingleChannelElement *sce);
    switch (ac->oc[1].m4ac.object_type) {
    case AOT_ER_AAC_LD:
        imdct_and_window = imdct_and_windowing_ld;
        break;
    case AOT_ER_AAC_ELD:
        imdct_and_window = imdct_and_windowing_eld;
        break;
    default:
        imdct_and_window = ac->imdct_and_windowing;
    }
    for (type = 3; type >= 0; type--) {
        for (i = 0; i < MAX_ELEM_ID; i++) {
            ChannelElement *che = ac->che[type][i];
            if (che) {
                if (type <= TYPE_CPE)
                    apply_channel_coupling(ac, che, type, i, BEFORE_TNS, apply_dependent_coupling);
                if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP) {
                    if (che->ch[0].ics.predictor_present) {
                        if (che->ch[0].ics.ltp.present)
                            ac->apply_ltp(ac, &che->ch[0]);
                        if (che->ch[1].ics.ltp.present && type == TYPE_CPE)
                            ac->apply_ltp(ac, &che->ch[1]);
                    }
                }
                if (che->ch[0].tns.present)
                    ac->apply_tns(che->ch[0].coeffs, &che->ch[0].tns, &che->ch[0].ics, 1);
                if (che->ch[1].tns.present)
                    ac->apply_tns(che->ch[1].coeffs, &che->ch[1].tns, &che->ch[1].ics, 1);
                if (type <= TYPE_CPE)
                    apply_channel_coupling(ac, che, type, i, BETWEEN_TNS_AND_IMDCT, apply_dependent_coupling);
                if (type != TYPE_CCE || che->coup.coupling_point == AFTER_IMDCT) {
                    imdct_and_window(ac, &che->ch[0]);
                    if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP)
                        ac->update_ltp(ac, &che->ch[0]);
                    if (type == TYPE_CPE) {
                        imdct_and_window(ac, &che->ch[1]);
                        if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP)
                            ac->update_ltp(ac, &che->ch[1]);
                    }
                    if (ac->oc[1].m4ac.sbr > 0) {
                        ff_sbr_apply(ac, &che->sbr, type, che->ch[0].ret, che->ch[1].ret);
                    }
                }
                if (type <= TYPE_CCE)
                    apply_channel_coupling(ac, che, type, i, AFTER_IMDCT, apply_independent_coupling);
            }
        }
    }
}