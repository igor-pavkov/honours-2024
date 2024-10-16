Client::handleAdaptedHeader(HttpMsg *msg)
{
    if (abortOnBadEntry("entry went bad while waiting for adapted headers")) {
        // If the adapted response has a body, the ICAP side needs to know
        // that nobody will consume that body. We will be destroyed upon
        // return. Tell the ICAP side that it is on its own.
        HttpReply *rep = dynamic_cast<HttpReply*>(msg);
        assert(rep);
        if (rep->body_pipe != NULL)
            rep->body_pipe->expectNoConsumption();

        return;
    }

    HttpReply *rep = dynamic_cast<HttpReply*>(msg);
    assert(rep);
    debugs(11,5, HERE << this << " setting adapted reply to " << rep);
    setFinalReply(rep);

    assert(!adaptedBodySource);
    if (rep->body_pipe != NULL) {
        // subscribe to receive adapted body
        adaptedBodySource = rep->body_pipe;
        // assume that ICAP does not auto-consume on failures
        const bool result = adaptedBodySource->setConsumerIfNotLate(this);
        assert(result);
    } else {
        // no body
        if (doneWithAdaptation()) // we may still be sending virgin response
            handleAdaptationCompleted();
    }
}