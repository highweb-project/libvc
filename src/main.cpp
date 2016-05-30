#include "vc.h"
using namespace vc;

#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
using namespace std;
using namespace chrono;

#define BUFFER_SIZE 10240
#define INCREMENT_PASSES 50000
#define RUNS 5

int main()
{
    DevicePool devicePool;
    for (Device &device : devicePool.getDevices()) {
        cout << "[" << device.getName() << "]" << endl;

        try {
            Buffer buffer(device, sizeof(double) * BUFFER_SIZE);
            buffer.fill(0);

            string path = "./shaders/simple.spv";
#if defined (_WIN32)
            path = "..\\..\\shaders\\simple.spv";
#endif
            Program program(device, path.c_str(), {BUFFER});
            Arguments args(program, {buffer});

            CommandBuffer commands(device, program, args);
                for (int i = 0; i < INCREMENT_PASSES; i++) {
                    commands.dispatch(BUFFER_SIZE / 256);
                    commands.barrier();
                }
            commands.end();

            // time the execution on the GPU
            steady_clock::time_point start = steady_clock::now();
            for (int i = 0; i < RUNS; i++) {
                device.submit(commands);
                device.wait();
            }
            cout << "elapsed: " << duration_cast<milliseconds>(steady_clock::now() - start).count() << "ms" << endl;

            double results[BUFFER_SIZE];
            buffer.download(results);

            buffer.destroy();
            args.destroy();
			program.destroy();
            commands.destroy();
            device.destroy();

            cout << "Scalar is: " << results[0] << endl;
            for (int i = 1; i < BUFFER_SIZE; i++) {
                if (results[i] != results[i - 1]) {
                    cout << "Corruption at " << i << ": " << results[i] << " != " << results[i - 1] << endl;
                    getchar();
                    return -1;
                }
            }
        } catch(vc::Error e) {
            cout << "vc::Error thrown" << endl;
        }
    }

    cout << "OK" << endl;
    getchar();
    return 0;
}
