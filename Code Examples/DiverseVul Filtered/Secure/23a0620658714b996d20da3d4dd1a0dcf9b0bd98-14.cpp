bool SQInstance::GetMetaMethod(SQVM* SQ_UNUSED_ARG(v),SQMetaMethod mm,SQObjectPtr &res)
{
    if(sq_type(_class->_metamethods[mm]) != OT_NULL) {
        res = _class->_metamethods[mm];
        return true;
    }
    return false;
}