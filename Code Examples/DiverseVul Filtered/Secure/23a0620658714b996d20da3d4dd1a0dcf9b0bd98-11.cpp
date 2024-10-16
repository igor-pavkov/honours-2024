SQInstance *SQClass::CreateInstance()
{
    if(!_locked) Lock();
    return SQInstance::Create(_opt_ss(this),this);
}