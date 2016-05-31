default:
	g++ -O2 -s -std=c++11 src/*.cpp -I include -L lib -lvulkan -o libvc_test
run:
	LD_LIBRARY_PATH=lib ./libvc_test
clean:
	rm -f libvc_test
