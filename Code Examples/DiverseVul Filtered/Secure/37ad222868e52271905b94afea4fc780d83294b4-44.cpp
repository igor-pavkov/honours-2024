void usleep2(const unsigned long microsec)
{
    disablesignals();
    usleep(microsec);
    enablesignals();
}