ACLSNMPCommunityStrategy::match (ACLData<MatchType> * &data, ACLFilledChecklist *checklist, ACLFlags &)
{
    return data->match (checklist->snmp_community);
}