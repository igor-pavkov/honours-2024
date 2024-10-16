void AppLayerProtoDetectReset(Flow *f)
{
    FlowUnsetChangeProtoFlag(f);
    FLOW_RESET_PM_DONE(f, STREAM_TOSERVER);
    FLOW_RESET_PM_DONE(f, STREAM_TOCLIENT);
    FLOW_RESET_PP_DONE(f, STREAM_TOSERVER);
    FLOW_RESET_PP_DONE(f, STREAM_TOCLIENT);
    FLOW_RESET_PE_DONE(f, STREAM_TOSERVER);
    FLOW_RESET_PE_DONE(f, STREAM_TOCLIENT);
    f->probing_parser_toserver_alproto_masks = 0;
    f->probing_parser_toclient_alproto_masks = 0;

    AppLayerParserStateCleanup(f, f->alstate, f->alparser);
    f->alstate = NULL;
    f->alparser = NULL;
    f->alproto    = ALPROTO_UNKNOWN;
    f->alproto_ts = ALPROTO_UNKNOWN;
    f->alproto_tc = ALPROTO_UNKNOWN;
}