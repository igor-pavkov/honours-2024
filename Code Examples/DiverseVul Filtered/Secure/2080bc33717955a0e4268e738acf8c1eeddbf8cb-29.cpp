AVHWAccel *av_hwaccel_next(const AVHWAccel *hwaccel)
{
    return hwaccel ? hwaccel->next : first_hwaccel;
}