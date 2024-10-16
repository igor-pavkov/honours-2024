Client::noteAdaptationAclCheckDone(Adaptation::ServiceGroupPointer group)
{
    adaptationAccessCheckPending = false;

    if (abortOnBadEntry("entry went bad while waiting for ICAP ACL check"))
        return;

    // TODO: Should non-ICAP and ICAP REPMOD pre-cache paths check this?
    // That check now only happens on REQMOD pre-cache and REPMOD post-cache, in processReplyAccess().
    if (virginReply()->expectedBodyTooLarge(*request)) {
        sendBodyIsTooLargeError();
        return;
    }
    // TODO: Should we check receivedBodyTooLarge as well?

    if (!group) {
        debugs(11,3, HERE << "no adapation needed");
        setFinalReply(virginReply());
        processReplyBody();
        return;
    }

    startAdaptation(group, originalRequest());
    processReplyBody();
}