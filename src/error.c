#include "error.h"

#include <string.h>
#include <stdio.h>

#include "globals.h"

mrerror mrerror_new(const char const *message)
{
	static char msgbuf[2][512];
	static int msgidx = 0;

	mrerror ret = { 0 };

	strncpy(msgbuf[msgidx], message, 512);
	ret.err = 1;
	ret.msg = msgbuf[msgidx];

	msgidx ^= 1;

	return ret;
}

mrerror nilerr()
{
	return (mrerror){ 0 };
}

void mrerror_show(mrerror err)
{
#if defined(WINDOWS_OS) && defined(GUI_ERRORS)
	wchar_t message[512];
	mbstowcs_s(NULL, message, strlen(err.msg) + 1, err.msg, _TRUNCATE);

	MessageBoxW(NULL, message, L"mr err",
		    MB_ICONERROR | MB_OK | MB_DEFBUTTON2);
#else
	fprintf(stderr, "error: %s\n", err.msg);
#endif
}
