int MqttClient_Ping(MqttClient *client)
{
    MqttPing ping;
    XMEMSET(&ping, 0, sizeof(ping));
    return MqttClient_Ping_ex(client, &ping);
}