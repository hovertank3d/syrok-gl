#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "cglm/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <unistd.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.h>

#include "error.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "globals.h"

#define PATHBUF_SIZE 512
#define MAX_PATH 128

#if defined(WINDOWS_OS)
#define realpath_(path, buffer)    \
	char **lppPart = { NULL }; \
	GetFullPathName(path, PATHBUF_SIZE, buffer, lppPart);

#define mkdir_p(path) CreateDirectoryA(path, 0)
#else
#define realpath_(path, result) realpath(path, result)
#define mkdir_p(path) mkdir(path, S_IRWXU)
#endif

struct application {
	char texture_path[PATHBUF_SIZE];
	int w, h;

	shader doom_shader;
	shader line_shader;
	mesh screen;
	texture img;

	char *out_path;

	GLFWwindow *wnd;
};

mrerror initGLFW(struct application *app)
{
	const char *err;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(app->w, app->h, "mr", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		glfwGetError(&err);
		return mrerror_new(err);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	app->wnd = window;
	return nilerr();
}

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
		    GLsizei length, const GLchar *message,
		    const void *userParam)
{
	fprintf(stderr, "OpenGL Debug Message: %s\n", message);
}

mrerror initGL(struct application *app, char *image_path)
{
	mrerror err;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return mrerror_new("Can't init GL");
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);

	glClearColor(0.5, 0.1, 0.4, 1.0);

	glViewport(0, 0, app->w, app->h);

	// todo refactor this lmao
	if (texture_find(&(app->img), image_path).err) {
		return mrerror_new("const char *message");
	}

	app->screen = mesh_new_quad();
	app->screen.textures[0] = app->img;

	err = shader_load(&app->doom_shader, "assets/vert_quad.glsl",
			  "assets/syrok.glsl");
	if (err.err) {
		return err;
	}

	return err;
}

void render_frame(struct application app)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(app.doom_shader);
	
	glUniform2i(glGetUniformLocation(app.doom_shader, "screen_size"),
 		app.w,
 		app.h);

	mesh_render_quad(app.screen, app.doom_shader);
	//todo
}

mrerror save_image(struct application app)
{
	// Make the BYTE array, factor of 3 because it's RBG.
	uint8_t* pixels = malloc(4 * app.w * app.h);
	glReadPixels(0, 0, app.w, app.h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_write_png(app.out_path, app.w, app.h, 4, pixels, 4*app.w);
}

void app_main(struct application app)
{
	char filename[64];
	render_frame(app);

	glfwSwapBuffers(app.wnd);

	save_image(app);

}

int main(int argc, char **argv)
{
	mrerror err;
	struct application app;

	memset(&app, 0, sizeof(struct application));

	if (argc != 3) {
		return 0;
	}

	char *in_path, *out_path;
	in_path = argv[1];
	app.out_path = argv[2];

	int x, y, n;
	stbi_load(in_path, &x, &y, &n, 4); 

	app.w = x;
	app.h = y;

	err = initGLFW(&app);
	if (err.err) {
		mrerror_show(err);
		return 1;
	}

	err = initGL(&app, in_path);
	if (err.err) {
		mrerror_show(err);
		return 1;
	}

	app_main(app);
}
