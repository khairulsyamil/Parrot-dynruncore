#include <parrot/parrot.h>
#include <parrot/runcore_api.h>
#include <stdio.h>

Parrot_PMC Parrot_lib_TestRuncore_load(Parrot_Interp interp) {
	printf("I am loaded!\n");
	return Parrot_pmc_new(interp, enum_class_ParrotLibrary);
}

void Parrot_lib_TestRuncore_init(Parrot_Interp interp, Parrot_PMC me) {
	printf("I am initializing!\n");
}
// ^ the above 2 functions currently serve no purpose.


opcode_t *
TestRuncore_runops(Parrot_Interp interp, Parrot_runcore_t *runcore, opcode_t *pc)
{	
	printf("Running ops!\n");
	//This is a copy of the runops function in doctor.c

    while (pc) {
		Parrot_pcc_set_pc(interp, CURRENT_CONTEXT(interp), pc);
        DO_OP(pc, interp);
    }
	
    return pc;
	
}

void
TestRuncore_init (Parrot_Interp interp,  oplib_init_f OpLibInit) {
	printf("Initializing runcore!\n");
	
	Parrot_runcore_t * const coredata = mem_gc_allocate_zeroed_typed(interp, Parrot_runcore_t);
	
	coredata->name			= string_from_literal(interp, "TestRuncore");
	coredata->id			= PARROT_FUNCTION_CORE; // <- Just use this constant for now.
	coredata->opinit		= OpLibInit; // <- For now, use the function pointer that was passed.
				    		             //    Todo: Figure out how to compile in core_ops.c, then this runcore is standalone.
	coredata->runops		= TestRuncore_runops;
	coredata->prepare_run	= NULL; // Todo: Find out what the three below does.
	coredata->destroy		= NULL;
	coredata->flags			= 0;
	
	PARROT_RUNCORE_FUNC_TABLE_SET(coredata);
	// ^ Macro defined in runcore_api.h
	//   Sets the op_func table.
	
	Parrot_runcore_register(interp, coredata);
	
	//Set this runcore as to be the one running the show.
	Parrot_runcore_switch(interp, coredata->name); // <- Reuse the string.
}