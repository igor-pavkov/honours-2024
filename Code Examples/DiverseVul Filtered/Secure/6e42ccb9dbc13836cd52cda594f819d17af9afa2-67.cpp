static void pop_output_configuration(AACContext *ac) {
    if (ac->oc[1].status != OC_LOCKED && ac->oc[0].status != OC_NONE) {
        ac->oc[1] = ac->oc[0];
        ac->avctx->channels = ac->oc[1].channels;
        ac->avctx->channel_layout = ac->oc[1].channel_layout;
        output_configure(ac, ac->oc[1].layout_map, ac->oc[1].layout_map_tags,
                         ac->oc[1].status, 0);
    }
}