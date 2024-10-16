SQInteger SQClass::Next(const SQObjectPtr &refpos, SQObjectPtr &outkey, SQObjectPtr &outval)
{
    SQObjectPtr oval;
    SQInteger idx = _members->Next(false,refpos,outkey,oval);
    if(idx != -1) {
        if(_ismethod(oval)) {
            outval = _methods[_member_idx(oval)].val;
        }
        else {
            SQObjectPtr &o = _defaultvalues[_member_idx(oval)].val;
            outval = _realval(o);
        }
    }
    return idx;
}