Client::handleAdaptationBlocked(const Adaptation::Answer &answer)
{
    debugs(11,5, HERE << answer.ruleId);

    if (abortOnBadEntry("entry went bad while ICAP aborted"))
        return;

    if (!entry->isEmpty()) { // too late to block (should not really happen)
        if (request)
            request->detailError(ERR_ICAP_FAILURE, ERR_DETAIL_RESPMOD_BLOCK_LATE);
        abortAll("late adaptation block");
        return;
    }

    debugs(11,7, HERE << "creating adaptation block response");

    err_type page_id =
        aclGetDenyInfoPage(&Config.denyInfoList, answer.ruleId.termedBuf(), 1);
    if (page_id == ERR_NONE)
        page_id = ERR_ACCESS_DENIED;

    ErrorState *err = new ErrorState(page_id, Http::scForbidden, request);
    err->detailError(ERR_DETAIL_RESPMOD_BLOCK_EARLY);
    fwd->fail(err);
    fwd->dontRetry(true);

    abortOnData("timely adaptation block");
}