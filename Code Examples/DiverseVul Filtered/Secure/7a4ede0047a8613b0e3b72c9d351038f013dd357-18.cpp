static void oss_init_per_direction(AudiodevOssPerDirectionOptions *opdo)
{
    if (!opdo->has_try_poll) {
        opdo->try_poll = true;
        opdo->has_try_poll = true;
    }
}