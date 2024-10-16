int AppLayerProtoDetectConfProtoDetectionEnabled(const char *ipproto,
                                                 const char *alproto)
{
    SCEnter();

    BUG_ON(ipproto == NULL || alproto == NULL);

    int enabled = 1;
    char param[100];
    ConfNode *node;
    int r;

#ifdef AFLFUZZ_APPLAYER
    goto enabled;
#endif
    if (RunmodeIsUnittests())
        goto enabled;

    r = snprintf(param, sizeof(param), "%s%s%s", "app-layer.protocols.",
                 alproto, ".enabled");
    if (r < 0) {
        SCLogError(SC_ERR_FATAL, "snprintf failure.");
        exit(EXIT_FAILURE);
    } else if (r > (int)sizeof(param)) {
        SCLogError(SC_ERR_FATAL, "buffer not big enough to write param.");
        exit(EXIT_FAILURE);
    }

    node = ConfGetNode(param);
    if (node == NULL) {
        SCLogDebug("Entry for %s not found.", param);
        r = snprintf(param, sizeof(param), "%s%s%s%s%s", "app-layer.protocols.",
                     alproto, ".", ipproto, ".enabled");
        if (r < 0) {
            SCLogError(SC_ERR_FATAL, "snprintf failure.");
            exit(EXIT_FAILURE);
        } else if (r > (int)sizeof(param)) {
            SCLogError(SC_ERR_FATAL, "buffer not big enough to write param.");
            exit(EXIT_FAILURE);
        }

        node = ConfGetNode(param);
        if (node == NULL) {
            SCLogDebug("Entry for %s not found.", param);
            goto enabled;
        }
    }

    if (node->val) {
        if (ConfValIsTrue(node->val)) {
            goto enabled;
        } else if (ConfValIsFalse(node->val)) {
            goto disabled;
        } else if (strcasecmp(node->val, "detection-only") == 0) {
            goto enabled;
        }
    }

    /* Invalid or null value. */
    SCLogError(SC_ERR_FATAL, "Invalid value found for %s.", param);
    exit(EXIT_FAILURE);

 disabled:
    enabled = 0;
 enabled:
    SCReturnInt(enabled);
}