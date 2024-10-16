static void oss_fini_in (HWVoiceIn *hw)
{
    OSSVoiceIn *oss = (OSSVoiceIn *) hw;

    oss_anal_close (&oss->fd);
}