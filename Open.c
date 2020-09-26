#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	void *handle = dlopen(argv[1], RTLD_NOW);
	if(!handle){
		fprintf(stderr,"dlopen: %s\n", dlerror());
		exit(1);
	}
	void(*func)() = dlsym(handle, argv[2]);
	if(!func){
		fprintf(stderr, "dlsym: %s\n", dlerror());
		exit(1);
	}
	func();
	dlclose(handle);
	return 0;
}

