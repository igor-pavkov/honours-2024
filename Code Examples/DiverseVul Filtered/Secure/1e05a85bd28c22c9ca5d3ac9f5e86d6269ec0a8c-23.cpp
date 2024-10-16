bool Client::startRequestBodyFlow()
{
    HttpRequest *r = originalRequest();
    assert(r->body_pipe != NULL);
    requestBodySource = r->body_pipe;
    if (requestBodySource->setConsumerIfNotLate(this)) {
        debugs(11,3, HERE << "expecting request body from " <<
               requestBodySource->status());
        return true;
    }

    debugs(11,3, HERE << "aborting on partially consumed request body: " <<
           requestBodySource->status());
    requestBodySource = NULL;
    return false;
}