static int lua_ap_started(request_rec* r) 
{
    return (int)(ap_scoreboard_image->global->restart_time / 1000000);
}