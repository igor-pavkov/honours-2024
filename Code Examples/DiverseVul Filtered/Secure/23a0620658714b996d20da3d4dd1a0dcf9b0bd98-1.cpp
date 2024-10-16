void SQInstance::Finalize()
{
    SQUnsignedInteger nvalues = _class->_defaultvalues.size();
    __ObjRelease(_class);
    _NULL_SQOBJECT_VECTOR(_values,nvalues);
}