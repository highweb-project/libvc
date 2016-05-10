default:
#	g++ -O2 -s -std=c++11 case1_vulkan.cpp -I ../include -L ../lib -l:libvulkan.so.1 -o case1_vulkan
	g++ -O2 -s -std=c++11 case1_opencl.cpp -I ../include -L ../lib -l:libOpenCL.so.1 -o case1_opencl
	g++ -O2 -s -std=c++11 case1_opengl.cpp -I ../include -L ../lib -lOpenGL -lglut -o case1_opengl
run:
#	LD_LIBRARY_PATH=../lib ./case1_vulkan
	LD_LIBRARY_PATH=../lib ./case1_opencl
	LD_LIBRARY_PATH=../lib ./case1_opengl
clean:
#	rm -f case1_vulkan
	rm -f case1_opencl
	rm -f case1_opengl
