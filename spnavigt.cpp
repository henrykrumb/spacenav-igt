#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <spnav.h>

#include <igtl/igtlServerSocket.h>
#include <igtl/igtlTransformMessage.h>


static bool running;
igtl::Socket::Pointer client_socket;

void sig(int s)
{
    running = false;
}

int main(void)
{
    spnav_event sev;

    signal(SIGINT, sig);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to spacenavd\n");
        return 1;
    }

    int port = 18945;
    int timeout = 1000;
    running = true;
    auto server_socket = igtl::ServerSocket::New();
    int status = server_socket->CreateServer(port);
    
    float matrix[4][4] = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0},
    };

    if (status < 0)
    {
        std::cerr << "Cannot create a server socket." << std::endl;
        std::cerr << "    status: " << status << std::endl;
        exit(EXIT_FAILURE);
    }

    bool dirty = true;
    while (running)
    {
        client_socket = server_socket->WaitForConnection(timeout);

        while (running && client_socket.IsNotNull() && client_socket->GetConnected())
        {

            if (spnav_poll_event(&sev))
            {
                if (sev.type == SPNAV_EVENT_MOTION)
                {
                    matrix[0][3] = (float)-sev.motion.x;
                    matrix[2][3] = (float)sev.motion.y;
                    matrix[1][3] = (float)-sev.motion.z;

                    printf("got motion event: t(%d, %d, %d) ", sev.motion.x, sev.motion.y, sev.motion.z);
                    printf("r(%d, %d, %d)\n", sev.motion.rx, sev.motion.ry, sev.motion.rz);
                    dirty = true;
                }
                else if (sev.type == SPNAV_EVENT_BUTTON)
                {
                    printf("got button %s event b(%d)\n", sev.button.press ? "press" : "release", sev.button.bnum);
                }
            }

            if (dirty)
            {
                auto transform_message = igtl::TransformMessage::New();
                transform_message->SetDeviceName("SpaceMouse");
                transform_message->SetMatrix(matrix);
                transform_message->Pack();
                if (!client_socket->Send(transform_message->GetPackPointer(), transform_message->GetPackSize()))
                {
                    // ...
                }
                dirty = false;
            }
        }
    }

    spnav_close();
    return 0;
}
