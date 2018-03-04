#include <add_include.h>
#include "App/App.h"

int main()
{
	int ret = 0;

	App app;
	
	ret = app.Init();
	if (ret != 0) {
		char log[64] = { 0 };
		snprintf(log, 64, "init err:%d", ret);
		printf(log);
		perror(log);
		return ret;
	}

	ret = app.Main();
	if (ret != 0) {
		char log[64] = { 0 };
		snprintf(log, 64, "main err:%d", ret);
		printf(log);
		perror(log);
	}

	ret = app.Finish();
	if (ret != 0) {
		char log[64] = { 0 };
		snprintf(log, 64, "finish err:%d", ret);
		printf(log);
		perror(log);
	}

	return ret;
}

/*EOF*/
