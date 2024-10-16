bool OpenConnectionTask::checkCapabilitiesResult(const Responses::State *const resp)
{
    if (resp->tag.isEmpty())
        return false;

    if (resp->tag == capabilityCmd) {
        if (!model->accessParser(parser).capabilitiesFresh) {
            logout(tr("Server did not provide useful capabilities"));
            return true;
        }
        if (resp->kind != Responses::OK) {
            logout(tr("CAPABILITIES command has failed"));
        }
        return true;
    }

    return false;
}