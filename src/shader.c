#include "shader.h"

#include <errno.h>
#include <glad/glad.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>

#include "error.h"

mrerror shader_compile(uint32_t *s, int type, const char *shader)
{
	*s = glCreateShader(type);
	glShaderSource(*s, 1, (const GLchar *const *)&shader, NULL);
	glCompileShader(*s);

	int success;
	char infoLog[512];
	glGetShaderiv(*s, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(*s, 512, NULL, infoLog);
		return mrerror_new(infoLog);
	}

	return nilerr();
}

mrerror shader_load(shader *s, const char *vert_path, const char *frag_path)
{
	FILE *v, *f;
	char *vert, *frag;
	struct stat buf;
	mrerror err;

	v = fopen(vert_path, "r");
	f = fopen(frag_path, "r");

	if (v == NULL || f == NULL) {
		return mrerror_new(strerror(errno));
	}

	fstat(fileno(v), &buf);
	vert = calloc(1, buf.st_size+1);
	if (fread(vert, sizeof(char), buf.st_size, v) != buf.st_size) {
		return mrerror_new(strerror(errno));
	}

	fstat(fileno(f), &buf);
	frag = calloc(1, buf.st_size+1);	
	if (fread(frag, sizeof(char), buf.st_size, f) != buf.st_size) {
		return mrerror_new(strerror(errno));
	}

	err = shader_new(s, vert, frag);
	free(vert);
	free(frag);
	return err;
}

mrerror shader_new(shader *s, const char *vert, const char *frag)
{
	uint32_t verts, frags;

	shader_compile(&verts, GL_VERTEX_SHADER, vert);
	shader_compile(&frags, GL_FRAGMENT_SHADER, frag);

	*s = glCreateProgram();
	glAttachShader(*s, verts);
	glAttachShader(*s, frags);
	glLinkProgram(*s);

	int success;
	char infoLog[512];
	glGetProgramiv(*s, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(*s, 512, NULL, infoLog);
		return mrerror_new(infoLog);
	}

	glDeleteShader(verts);
	glDeleteShader(frags);

	return nilerr();
}
