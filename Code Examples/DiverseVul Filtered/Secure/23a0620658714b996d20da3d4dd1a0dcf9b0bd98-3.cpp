bool SQClass::SetAttributes(const SQObjectPtr &key,const SQObjectPtr &val)
{
    SQObjectPtr idx;
    if(_members->Get(key,idx)) {
        if(_isfield(idx))
            _defaultvalues[_member_idx(idx)].attrs = val;
        else
            _methods[_member_idx(idx)].attrs = val;
        return true;
    }
    return false;
}