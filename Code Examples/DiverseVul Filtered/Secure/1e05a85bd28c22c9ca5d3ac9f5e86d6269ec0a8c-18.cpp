Client::Client(FwdState *theFwdState): AsyncJob("Client"),
    completed(false),
    currentOffset(0),
    responseBodyBuffer(NULL),
    fwd(theFwdState),
    requestSender(NULL),
#if USE_ADAPTATION
    adaptedHeadSource(NULL),
    adaptationAccessCheckPending(false),
    startedAdaptation(false),
#endif
    receivedWholeRequestBody(false),
    doneWithFwd(nullptr),
    theVirginReply(NULL),
    theFinalReply(NULL)
{
    entry = fwd->entry;
    entry->lock("Client");

    request = fwd->request;
    HTTPMSGLOCK(request);
}