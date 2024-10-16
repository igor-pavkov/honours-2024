int MqttClient_NetDisconnect(MqttClient *client)
{
    return MqttSocket_Disconnect(client);
}