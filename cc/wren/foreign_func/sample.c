#include "wren.h"
#include <stdio.h>
#include <string.h>

static const char* text =
"import \"random\" for Random\n"
"import \"foo\" for Foo\n"
//"import \"bar\" for Bar\n"
"\n"
"System.print(\"abcd\".byteCount_)"
"\n"
"var random = Random.new(12345)\n"
"var list = (1..5).toList\n"
"random.shuffle(list)\n"
"System.print(list) //> [3, 2, 4, 1, 5]\n"
"\n"
"Foo.say()\n"
"Foo.hey()\n"
//"Foo.hey2(\"aa\")\n"
"Foo.hey2(10)\n"
;

static const char* foo_class_text = 
"class Foo {\n"
"  static say() {\n"
"    System.print(\"foo!\")\n"
"  }\n"
"  foreign static hey()\n"
"  foreign static hey2(s)\n"
"}\n"
;

static void write_func(WrenVM* vm, const char* text);
static void report_func(WrenErrorType type, const char* module, int line, const char* message);
static const char *wren_type2name(WrenType type);

char* sample_load_module(WrenVM* vm, const char* name);
WrenForeignMethodFn sample_foreign_method(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature);
WrenForeignClassMethods sample_foreign_class(WrenVM* vm, const char* module, const char* className);

void foo_hey(WrenVM* vm);
void foo_hey2(WrenVM* vm);

int main(int argc, char** argv)
{
	WrenConfiguration config; 
	WrenVM* vm;

	wrenInitConfiguration(&config);

	config.writeFn = write_func;
	config.errorFn = report_func;
	config.loadModuleFn = sample_load_module;
	config.bindForeignMethodFn = sample_foreign_method;
	config.bindForeignClassFn = sample_foreign_class;
	config.bindForeignClassFn = NULL;

	vm = wrenNewVM(&config);

	WrenInterpretResult result = wrenInterpret(vm, text);
	if (result == WREN_RESULT_SUCCESS) {
		printf("success\n");
	} else {
		printf("return code = %d\n", result);
	}

	wrenFreeVM(vm);

	return 0;
}

static void write_func(WrenVM* vm, const char* text)
{
	puts(">>>>");
	printf("%s\n", text);
	fflush(stdout);
}

static void report_func(WrenErrorType type, const char* module, int line, const char* message)
{
	switch (type) {
	case WREN_ERROR_COMPILE:
		fprintf(stderr, "[%s line %d] %s\n", module, line, message);
		break;
    case WREN_ERROR_RUNTIME:
		fprintf(stderr, "%s\n", message);
		break;
	case WREN_ERROR_STACK_TRACE:
		fprintf(stderr, "[%s line %d] in %s\n", module, line, message);
		break;
	default:
		break;
	}
}

char* sample_load_module(WrenVM* vm, const char* name)
{
	printf("load) name=[%s]\n", name);

	if (strcmp(name, "foo") == 0) {
		return (char*)foo_class_text;
	}

	return NULL;
}

WrenForeignMethodFn sample_foreign_method(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature)
{
	WrenForeignMethodFn method = NULL;

	printf("foreign method) module=[%s], className=[%s], signature=[%s]\n", module, className, signature);

	if (strcmp("foo", module) == 0 && strcmp("Foo", className) == 0 && strcmp("hey()", signature) == 0) {
		return foo_hey;
	}
	if (strcmp("foo", module) == 0 && strcmp("Foo", className) == 0 && strcmp("hey2(_)", signature) == 0) {
		return foo_hey2;
	}

	return method;
}

WrenForeignClassMethods sample_foreign_class(WrenVM* vm, const char* module, const char* className)
{
	WrenForeignClassMethods methods;

	printf("foreign class) module=[%s], className=[%s]\n", module, className);

	methods.allocate = NULL;
	methods.finalize = NULL;

	return methods;
}

void foo_hey(WrenVM* vm)
{
	int argc = wrenGetSlotCount(vm);

	printf("foo> [%d]\n", argc);
	printf("foo> Hey!!\n");
}

void foo_hey2(WrenVM* vm)
{
	int argc = wrenGetSlotCount(vm);
	WrenType type;

	type = wrenGetSlotType(vm, 1);

	printf("foo2> [%d]\n", argc);
	printf("foo2> argv[1] = [%s]\n", wren_type2name(type));
}

static const char *wren_type2name(WrenType type)
{
	char *name = NULL;
	switch (type) {
	case WREN_TYPE_BOOL:
		name = "bool";
		break;
	case WREN_TYPE_NUM:
		name = "num";
		break;
	case WREN_TYPE_FOREIGN:
		name = "foreign";
		break;
	case WREN_TYPE_LIST:
		name = "list";
		break;
	case WREN_TYPE_NULL:
		name = "null";
		break;
	case WREN_TYPE_STRING:
		name = "string";
		break;
	case WREN_TYPE_UNKNOWN:
		name = "unknown";
		break;
	default:
		break;
	}

	return name;
}

