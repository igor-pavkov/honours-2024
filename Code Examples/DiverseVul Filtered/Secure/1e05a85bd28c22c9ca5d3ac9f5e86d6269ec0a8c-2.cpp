Client::adaptOrFinalizeReply()
{
#if USE_ADAPTATION
    // TODO: merge with client side and return void to hide the on/off logic?
    // The callback can be called with a NULL service if adaptation is off.
    adaptationAccessCheckPending = Adaptation::AccessCheck::Start(
                                       Adaptation::methodRespmod, Adaptation::pointPreCache,
                                       originalRequest(), virginReply(), fwd->al, this);
    debugs(11,5, HERE << "adaptationAccessCheckPending=" << adaptationAccessCheckPending);
    if (adaptationAccessCheckPending)
        return;
#endif

    setFinalReply(virginReply());
}