static int defined_frame_rate(AVFormatContext *s, AVStream *st)
{
    AVRational rational_framerate = find_fps(s, st);
    int rate = 0;
    if (rational_framerate.den != 0)
        rate = av_q2d(rational_framerate);
    return rate;
}