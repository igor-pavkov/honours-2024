AppProto AppLayerProtoDetectGetProtoByName(const char *alproto_name)
{
    SCEnter();

    AppProto a;
    for (a = 0; a < ALPROTO_MAX; a++) {
        if (alpd_ctx.alproto_names[a] != NULL &&
            strlen(alpd_ctx.alproto_names[a]) == strlen(alproto_name) &&
            (SCMemcmp(alpd_ctx.alproto_names[a], alproto_name, strlen(alproto_name)) == 0))
        {
            SCReturnCT(a, "AppProto");
        }
    }

    SCReturnCT(ALPROTO_UNKNOWN, "AppProto");
}