#include <parrot/parrot.h>
#include <parrot/embed.h>
#include <parrot/imcc.h>
#include <parrot/dynext.h>
#include <parrot/runcore_api.h>

int main (int argc, char ** argv) {
	Parrot_String str;
	void (*runcore_init)(Parrot_Interp, oplib_init_f);

	//Initialise both interpreters.
	Parrot_Interp p = Parrot_new(NULL);
	
	//Initialise imcc for both interpreters.
	imcc_initialize(p);
	imcc_start_handling_flags(p);
	
	//Force the slow core.
	//We want to grab the opinit value of slow_core's runcore_t.
	//This is only temporary, as I have not figured out how to compile
	// core_ops.c together with TestRuncore to create a standalone runcore.
	Parrot_set_run_core(p, PARROT_SLOW_CORE);
	
	//Try loading the thing.
	str = string_from_literal(p, "TestRuncore");
	Parrot_PMC pmc = Parrot_load_lib(p, str, NULL);
	
	printf("A: %s\n", Parrot_str_to_cstring(p, (VTABLE_name(p, pmc))));
	printf("B: %s\n", Parrot_str_to_cstring(p, (VTABLE_get_string(p, pmc))));
	
	void * handle = VTABLE_get_pointer(p, pmc);
	void * dlsymfunc = Parrot_dlsym(handle, "TestRuncore_init");
	runcore_init = (void (*)(Parrot_Interp, op_lib_t *(*)(PARROT_INTERP, long))) D2FPTR(dlsymfunc);
	(*runcore_init)(p, p->run_core->opinit);
	
	//Run some code.
	imcc_run(p, argv[1], argc - 1, argv + 1);
	
	//Done.
	Parrot_destroy(p);

	return 0;
}