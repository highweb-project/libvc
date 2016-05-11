#define GL_GLEXT_PROTOTYPES

#include <chrono>
#include <iostream>
#include <vector>
#if defined _WIN32
#include <GL/glew.h>
#endif
#include <GL/glut.h>

using namespace std;
using namespace chrono;

#define BUFFER_SIZE		10240
#define INCREMENT_PASSES	50000
#define RUNS			5

static const GLchar * compute_source[] = {
	"#version 430 core						\n"
	"								\n"
	"layout(local_size_x = 256) in;					\n"
	"								\n"
	"layout(std430, binding = 0) buffer Pos {			\n"
	"	double Position[];					\n"
	"};								\n"
	"								\n"
	"void main() {							\n"
	"	Position[gl_GlobalInvocationID.x] += 2.0f;		\n"
	"}								\n"
};

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(0, 0);
	glutCreateWindow("Test Window");

#if defined _WIN32
	GLenum err = glewInit();
#endif

	GLuint compute_shader = 0;
	GLuint compute_program = 0;
	GLuint SSBO = 0;
	vector<GLdouble> data;
	for (int i = 0; i < BUFFER_SIZE; i++){
	    data.push_back(0.0f);
	}

	compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader, 1, compute_source, NULL);
	glCompileShader(compute_shader);

	compute_program = glCreateProgram();
	glAttachShader(compute_program, compute_shader);
	glLinkProgram(compute_program);
	glUseProgram(compute_program);

	glGenBuffers(1, &SSBO);	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BUFFER_SIZE * sizeof(GLdouble), &data[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
	for(int i = 0; i < RUNS; i++) {
		steady_clock::time_point start = steady_clock::now();
		for(int j = 0; j < INCREMENT_PASSES; j++) {
			glDispatchCompute(BUFFER_SIZE / 256, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		cout << i + 1 << ": " << duration_cast<milliseconds>(steady_clock::now() - start).count() << "ms" << endl;
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	GLdouble *ptr;
	ptr = (GLdouble *) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	data.clear();
	for (int i = 0; i < BUFFER_SIZE; i++){
	    data.push_back(ptr[i]);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	cout << "data[0] : " << data[0] << endl;

	if (int(data[0] + 0.5) != INCREMENT_PASSES * RUNS * 2) {
		cout << "Mismatching result!" << endl;
		return -3;
	}

	for (int i = 1; i < BUFFER_SIZE; i++) {
		if (data[i] != data[i - 1]) {
			cout << "Corruption at " << i << ": " << data[i] << " != " << data[i - 1] << endl;
			return -1;
		}
	}

	cout << "OK" << endl;
	getchar();

	return 0;
}

