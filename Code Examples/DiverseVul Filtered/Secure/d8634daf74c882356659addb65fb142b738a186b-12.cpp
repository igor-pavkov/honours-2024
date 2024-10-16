static DetectTransaction GetDetectTx(const uint8_t ipproto, const AppProto alproto,
        void *alstate, const uint64_t tx_id, void *tx_ptr, const int tx_end_state,
        const uint8_t flow_flags)
{
    const uint64_t detect_flags = AppLayerParserGetTxDetectFlags(ipproto, alproto, tx_ptr, flow_flags);
    if (detect_flags & APP_LAYER_TX_INSPECTED_FLAG) {
        SCLogDebug("%"PRIu64" tx already fully inspected for %s. Flags %016"PRIx64,
                tx_id, flow_flags & STREAM_TOSERVER ? "toserver" : "toclient",
                detect_flags);
        DetectTransaction no_tx = { NULL, 0, NULL, 0, 0, 0, 0, 0, };
        return no_tx;
    }

    const int tx_progress = AppLayerParserGetStateProgress(ipproto, alproto, tx_ptr, flow_flags);
    const int dir_int = (flow_flags & STREAM_TOSERVER) ? 0 : 1;
    DetectEngineState *tx_de_state = AppLayerParserGetTxDetectState(ipproto, alproto, tx_ptr);
    DetectEngineStateDirection *tx_dir_state = tx_de_state ? &tx_de_state->dir_state[dir_int] : NULL;
    uint64_t prefilter_flags = detect_flags & APP_LAYER_TX_PREFILTER_MASK;

    DetectTransaction tx = {
                            .tx_ptr = tx_ptr,
                            .tx_id = tx_id,
                            .de_state = tx_dir_state,
                            .detect_flags = detect_flags,
                            .prefilter_flags = prefilter_flags,
                            .prefilter_flags_orig = prefilter_flags,
                            .tx_progress = tx_progress,
                            .tx_end_state = tx_end_state,
                           };
    return tx;
}