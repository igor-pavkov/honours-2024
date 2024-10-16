Client::sendBodyIsTooLargeError()
{
    ErrorState *err = new ErrorState(ERR_TOO_BIG, Http::scForbidden, request);
    fwd->fail(err);
    fwd->dontRetry(true);
    abortOnData("Virgin body too large.");
}