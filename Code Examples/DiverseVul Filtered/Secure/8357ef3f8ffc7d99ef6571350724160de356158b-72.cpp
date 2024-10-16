int AppLayerProtoDetectPMRegisterPatternCI(uint8_t ipproto, AppProto alproto,
                                           const char *pattern,
                                           uint16_t depth, uint16_t offset,
                                           uint8_t direction)
{
    SCEnter();
    int r = 0;
    r = AppLayerProtoDetectPMRegisterPattern(ipproto, alproto,
                                                     pattern,
                                                     depth, offset,
                                                     direction,
                                                     0 /* !case-sensitive */);
    SCReturnInt(r);
}