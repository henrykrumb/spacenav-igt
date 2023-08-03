#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cmath>

#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

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
        {0.0f, (float)cos(a), (float)-sin(a), 0.0f},
        {0.0f, (float)sin(a), (float)cos(a), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};
    return QuadMatrix<4>(arr);
}

QuadMatrix<4> rotmatY(float angle)
{
    double a = M_PI * angle / 180.0;
    float arr[4][4] = {
        {(float)cos(a), 0.0f, (float)sin(a), 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {(float)-sin(a), 0.0f, (float)cos(a), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};
    return QuadMatrix<4>(arr);
}

QuadMatrix<4> rotmatZ(float angle)
{
    double a = M_PI * angle / 180.0;
    float arr[4][4] = {
        {(float)cos(a), (float)-sin(a), 0.0f, 0.0f},
        {(float)sin(a), (float)cos(a), 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};
    return QuadMatrix<4>(arr);
}

int main(int argc, char *argv[])
{
    spnav_event sev;

    signal(SIGINT, sig);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to spacenavd\n");
        return 1;
    }

    int port = 18944;
    int timeout = 1000;
    running = true;
    auto server_socket = igtl::ServerSocket::New();
    int status = server_socket->CreateServer(port);

    // parse command line args
    CLI::App app{"3DConnexion IGTLink Server"};
    app.add_option("-p,--port", port, "IGTLink server port");
    app.add_option("-t,--timeout", timeout, "Connection timeout");
    CLI11_PARSE(app, argc, argv);

    QuadMatrix<4> T, Rx, Ry, Rz;

    if (status < 0)
    {
        std::cerr << "Cannot create a server socket." << std::endl;
        std::cerr << "    status: " << status << std::endl;
        exit(EXIT_FAILURE);
    }

    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    bool dirty = true;
    float sensitivity = 0.005f;
    while (running)
    {
        client_socket = server_socket->WaitForConnection(timeout);

        while (running && client_socket.IsNotNull() && client_socket->GetConnected())
        {

            if (spnav_poll_event(&sev))
            {
                if (sev.type == SPNAV_EVENT_MOTION)
                {
                    T.set(0, 3, T.get(0, 3) - (float)-sev.motion.x * sensitivity);
                    T.set(2, 3, T.get(2, 3) - (float)sev.motion.y * sensitivity);
                    T.set(1, 3, T.get(1, 3) - (float)-sev.motion.z * sensitivity);

                    ax += sev.motion.rx * sensitivity;
                    ay -= sev.motion.ry * sensitivity;
                    az += sev.motion.rz * sensitivity;

                    printf("got motion event: t(%d, %d, %d) ", sev.motion.x, sev.motion.y, sev.motion.z);
                    printf("r(%d, %d, %d)\n", sev.motion.rx, sev.motion.ry, sev.motion.rz);
                    dirty = true;
                }
                else if (sev.type == SPNAV_EVENT_BUTTON)
                {
                    if (sev.button.bnum == 0)
                    {
                        T.identity();
                    }
                    else
                    {
                        ax = 0;
                        ay = 0;
                        az = 0;
                    }
                    dirty = true;
                    printf("got button %s event b(%d)\n", sev.button.press ? "press" : "release", sev.button.bnum);
                }
            }

            if (dirty)
            {
                Rx = rotmatX(ax);
                Ry = rotmatY(az);
                Rz = rotmatZ(ay);

                auto transform_message = igtl::TransformMessage::New();
                transform_message->SetDeviceName("SpaceMouse");
                float float_matrix[4][4];
                QuadMatrix<4> transform_matrix = Rz.multiply(Ry).multiply(Rx).multiply(T);
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
