int MqttClient_Disconnect(MqttClient *client)
{
    return MqttClient_Disconnect_ex(client, NULL);
}