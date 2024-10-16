bool SQClass::GetAttributes(const SQObjectPtr &key,SQObjectPtr &outval)
{
    SQObjectPtr idx;
    if(_members->Get(key,idx)) {
        outval = (_isfield(idx)?_defaultvalues[_member_idx(idx)].attrs:_methods[_member_idx(idx)].attrs);
        return true;
    }
    return false;
}