UrnState::fillChecklist(ACLFilledChecklist &checklist) const
{
    checklist.setRequest(request.getRaw());
    checklist.al = ale;
}