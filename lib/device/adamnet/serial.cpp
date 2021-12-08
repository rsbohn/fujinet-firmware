#ifdef BUILD_ADAM

#include <cstring>
#include "adamnet/serial.h"

adamSerial::adamSerial()
{
    Debug_printf("Serial Start\n");
    server = new fnTcpServer(1235, 1); // Run a TCP server on port 1235.
    server->begin(1235);
}

adamSerial::~adamSerial()
{
    server->stop();
    delete server;
}

void adamSerial::command_recv()
{
    if (response_len == 0 && client.available())
    {
        adamnet_response_nack();
        response_len = client.available() > 16 ? 16 : client.available();
        client.read(response, response_len);
    }
    else if (response_len > 0)
    {
        adamnet_response_ack();
    }
    else
    {
        adamnet_response_nack();
    }
}

void adamSerial::adamnet_control_status()
{
    if (!client.connected() && server->hasClient())
    {
        // Accept waiting connection
        client = server->available();
    }

    if (client.available())
    {
        status_msg[2] = 0x00;
        status_msg[3] = client.available() > 0;
    }

    adamnet_send(0x80 | _devnum);
    adamnet_send_buffer(status_msg, 4);
    adamnet_send(adamnet_checksum(status_msg, 4));
}

void adamSerial::adamnet_control_clr()
{
    if (response_len == 0)
    {
        adamnet_response_nack();
    }
    else
    {
        adamnet_send(0xB0 | _devnum);
        adamnet_send_length(response_len);
        adamnet_send_buffer(response, response_len);
        adamnet_send(adamnet_checksum(response, response_len));
        memset(response, 0, sizeof(response));
        response_len = 0;
    }
}

void adamSerial::adamnet_control_ready()
{
    AdamNet.wait_for_idle();
    adamnet_send(0x9E); // ACK.
}

void adamSerial::adamnet_control_send()
{
    uint16_t s = adamnet_recv_length();
    uint8_t c = adamnet_recv();

    if (client.connected())
    {
        for (uint16_t i = 0; i < s; i++)
            client.write(adamnet_recv());
    }

    adamnet_recv();
    adamnet_response_ack();
}

void adamSerial::adamnet_process(uint8_t b)
{
    unsigned char c = b >> 4;

    switch (c)
    {
    case MN_STATUS:
        adamnet_control_status();
        break;
    case MN_CLR:
        adamnet_control_clr();
        break;
    case MN_RECEIVE:
        command_recv();
        break;
    case MN_SEND:
        adamnet_control_send();
        break;
    case MN_READY:
        adamnet_control_ready();
        break;
    }
}

#endif /* BUILD_ADAM */