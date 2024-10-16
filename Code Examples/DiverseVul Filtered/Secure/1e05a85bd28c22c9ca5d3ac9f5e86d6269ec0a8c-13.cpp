Client::handledEarlyAdaptationAbort()
{
    if (entry->isEmpty()) {
        debugs(11,8, "adaptation failure with an empty entry: " << *entry);
        ErrorState *err = new ErrorState(ERR_ICAP_FAILURE, Http::scInternalServerError, request);
        err->detailError(ERR_DETAIL_ICAP_RESPMOD_EARLY);
        fwd->fail(err);
        fwd->dontRetry(true);
        abortAll("adaptation failure with an empty entry");
        return true; // handled
    }

    if (request) // update logged info directly
        request->detailError(ERR_ICAP_FAILURE, ERR_DETAIL_ICAP_RESPMOD_LATE);

    return false; // the caller must handle
}