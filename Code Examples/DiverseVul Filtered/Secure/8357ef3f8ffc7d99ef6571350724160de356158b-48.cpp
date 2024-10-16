static uint32_t AppLayerProtoDetectProbingParserGetMask(AppProto alproto)
{
    SCEnter();

    if (!(alproto > ALPROTO_UNKNOWN && alproto < ALPROTO_FAILED)) {
        SCLogError(SC_ERR_ALPARSER, "Unknown protocol detected - %"PRIu16,
                   alproto);
        exit(EXIT_FAILURE);
    }

    SCReturnUInt(1 << alproto);
}