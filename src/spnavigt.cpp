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
#include "utils.hpp"

static bool running;
igtl::Socket::Pointer client_socket;

void sig(int s)
{
    running = false;
}

int main(int argc, char *argv[])
{
    spnav_event sev;

    signal(SIGINT, sig);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "Failed to connect to spacenavd\n");
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
    bool lock = true;
    while (running)
    {
        client_socket = server_socket->WaitForConnection(timeout);

        while (running && client_socket.IsNotNull() && client_socket->GetConnected())
        {

            if (spnav_poll_event(&sev))
            {
                if (sev.type == SPNAV_EVENT_MOTION)
                {
                    float dx = (float)-sev.motion.x;
                    float dy = (float)sev.motion.y;
                    float dz = (float)-sev.motion.z;
                    float drx = (float)sev.motion.rx;
                    float dry = (float)sev.motion.ry;
                    float drz = (float)sev.motion.rz;

                    if (lock)
                    {
                        if (abs(drx) + abs(dry) + abs(drz) > 100)
                        {
                            dx = 0.0f;
                            dy = 0.0f;
                            dz = 0.0f;
                        }
                        if (abs(dx) < abs(dy) || abs(dx) < abs(dz))
                        {
                            dx = 0.0f;
                        }
                        if (abs(dy) < abs(dx) || abs(dy) < abs(dz))
                        {
                            dy = 0.0f;
                        }
                        if (abs(dz) < abs(dx) || abs(dz) < abs(dy))
                        {
                            dz = 0.0f;
                        }
                    }

                    T.set(0, 3, T.get(0, 3) - dx * sensitivity);
                    T.set(2, 3, T.get(2, 3) - dy * sensitivity);
                    T.set(1, 3, T.get(1, 3) - dz * sensitivity);

                    ax += drx * sensitivity;
                    ay -= dry * sensitivity;
                    az += drz * sensitivity;
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
