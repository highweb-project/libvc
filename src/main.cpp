#include "vc.h"
using namespace vc;

#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
using namespace std;
using namespace chrono;

int main()
{
    DevicePool devicePool;
    for (Device &device : devicePool.getDevices()) {
        cout << "[" << device.getName() << "]" << endl;

        try {
            Buffer buffer(device, sizeof(double) * 10240);
            buffer.fill(0);

            string path = "./shaders/simple.spv";
#if defined (_WIN32)
            path = "..\\..\\shaders\\simple.spv";
#endif
            Program program(device, path.c_str(), {BUFFER});
            Arguments args(program, {buffer});

            CommandBuffer commands(device, program, args);
                for (int i = 0; i < 50000; i++) {
                    commands.dispatch(40);
                    commands.barrier();
                }
            commands.end();

            // time the execution on the GPU
            for (int i = 0; i < 5; i++) {
                steady_clock::time_point submit = steady_clock::now();
                device.submit(commands);
				cout << "vkQueueSubmit elapsed : " << duration_cast<milliseconds>(steady_clock::now() - submit).count() << "ms" << endl;
				steady_clock::time_point wait = steady_clock::now();
                device.wait();
				cout << "vkQueueWaitIdle elapsed : " << duration_cast<milliseconds>(steady_clock::now() - wait).count() << "ms" << endl;
            }

            double results[10240];
            buffer.download(results);

            buffer.destroy();
            args.destroy();
            commands.destroy();
            device.destroy();

            cout << "Scalar is: " << results[0] << endl;
            for (int i = 1; i < 10240; i++) {
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
