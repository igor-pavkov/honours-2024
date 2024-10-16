static void register_audio_oss(void)
{
    audio_driver_register(&oss_audio_driver);
}