#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cmath>

#include <spnav.h>

#include <igtlServerSocket.h>
#include <igtlTransformMessage.h>

#include "matrix.hpp"

static bool running;
igtl::Socket::Pointer client_socket;

void sig(int s)
{
    running = false;
}

QuadMatrix<4> rotmatX(float angle)
{
    double a = M_PI * angle / 180.0;
    float arr[4][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, cos(a), -sin(a), 0.0f},
        {0.0f, sin(a), cos(a), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};
    return QuadMatrix<4>(arr);
}

QuadMatrix<4> rotmatY(float angle)
{
    double a = M_PI * angle / 180.0;
    float arr[4][4]  = {
        {cos(a), 0.0f, sin(a), 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {-sin(a), 0.0f, cos(a), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};
    return QuadMatrix<4>(arr);
}

QuadMatrix<4> rotmatZ(float angle)
{
    double a = M_PI * angle / 180.0;
    float arr[4][4] = {
        {cos(a), -sin(a), 0.0f, 0.0f},
        {sin(a), cos(a), 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};
    return QuadMatrix<4>(arr);
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

    QuadMatrix<4> T, Rx, Ry, Rz;

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
                    T.set(0, 3, (float)-sev.motion.x);
                    T.set(2, 3, (float)sev.motion.y);
                    T.set(1, 3, (float)-sev.motion.z);
                    Rx = rotmatX(sev.motion.rx);
                    Ry = rotmatY(sev.motion.rz);
                    Rz = rotmatZ(sev.motion.ry);

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
                float float_matrix[4][4];
                QuadMatrix<4> transform_matrix = Rx.multiply(Ry).multiply(Rz);
                transform_matrix.set(0, 3, T.get(0, 3));
                transform_matrix.set(2, 3, T.get(2, 3));
                transform_matrix.set(1, 3, T.get(1, 3));

                transform_matrix.toArray(float_matrix);
                transform_message->SetMatrix(float_matrix);
                transform_message->Pack();
                if (!client_socket->Send(transform_message->GetPackPointer(), transform_message->GetPackSize()))
                {
                }
                dirty = false;
            }
        }
    }

    spnav_close();
    return 0;
}
