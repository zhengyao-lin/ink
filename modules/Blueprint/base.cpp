#include "blueprint.h"
#include "error.h"
#include "core/object.h"
#include "core/native/general.h"
#include "core/gc/collect.h"

using namespace ink;

Ink_Object *InkMod_Blueprint_Base_If(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *ret;
	Ink_ArgcType i;

	if (!argc) {
		InkWarn_Blueprint_If_Argument_Require(engine);
		return NULL_OBJ;
	}

	i = 0;
	ret = cond = argv[0];
	if (isTrue(cond)) {
		i++;
		if (i < argc && argv[i]->type == INK_FUNCTION) {
			ret = argv[i]->call(engine, context);
		}
	} else {
		if (i + 1 < argc && argv[i + 1]->type == INK_FUNCTION) {
			i += 2;
		} else {
			i++;
		}
		for (; i < argc; i++) {
			if (argv[i]->type == INK_STRING) {
				if (as<Ink_String>(argv[i])->getValue() == "else") {
					if (++i < argc) {
						if (argv[i]->type == INK_STRING) {
							if (as<Ink_String>(argv[i])->getValue() == "if") {
								if (++i < argc) {
									if (argv[i]->type == INK_ARRAY) {
										if (as<Ink_Array>(argv[i])->value.size() && as<Ink_Array>(argv[i])->value[0]) {
											if (isTrue(as<Ink_Array>(argv[i])->value[0]->getValue())) {
												if (++i < argc) {
													if (argv[i]->type == INK_FUNCTION) {
														ret = argv[i]->call(engine, context);
														break;
													}
												} else {
													InkWarn_Blueprint_If_End_With_Else_If_Has_Condition(engine);
												}
											} else {
												i++;
												continue;
											}
										} else {
											InkWarn_Blueprint_If_Else_If_Has_No_Condition(engine);
											return ret;
										}
									} else {
										InkWarn_Blueprint_If_Else_If_Has_No_Condition(engine);
										return ret;
									}
								} else {
									InkWarn_Blueprint_If_End_With_Else_If(engine);
									return ret;
								}
							}
						} else if (argv[i]->type == INK_FUNCTION) {
							ret = argv[i]->call(engine, context);
						}
					} else {
						InkWarn_Blueprint_If_End_With_Else(engine);
						return ret;
					}
				}
			} else if (argv[i]->type == INK_FUNCTION) {
				ret = argv[i]->call(engine, context);
				break;
			}
		}
	}

	return ret;
}

Ink_Object *InkMod_Blueprint_Base_While(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *block;
	Ink_Object *ret;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();
	char trig = 0;

	if (argc < 1) {
		InkWarn_Blueprint_While_Argument_Require(engine);
		return NULL_OBJ;
	}

	cond = argv[0];
	block = argc > 1 ? argv[1] : NULL;
	if (cond->type != INK_FUNCTION) {
		InkWarn_Blueprint_While_Require_Reference_Cond(engine);
		return NULL_OBJ;
	} else if (block && block->type != INK_FUNCTION) {
		InkWarn_Blueprint_While_Require_Block(engine);
		return NULL_OBJ;
	}

	ret = NULL;
	while (isTrue(cond->call(engine, context))) {
		trig++;
		if (!(trig % 5))
			gc_engine->checkGC();
		
		if (block) {
			ret = block->call(engine, context);
			if (engine->getSignal() != INTER_NONE) {
				switch (engine->getSignal()) {
					case INTER_RETURN:
						return engine->getInterruptValue(); // fallthrough the signal
					case INTER_DROP:
					case INTER_BREAK:
						return engine->trapSignal(); // trap the signal
					case INTER_CONTINUE:
						engine->trapSignal(); // trap the signal, but do not return
						continue;
					default:
						return NULL_OBJ;
				}
			}
		}
	}

	return ret ? ret : NULL_OBJ;
}

Ink_Object *InkMod_Blueprint_Base_For(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *incr;
	Ink_Object *block;
	Ink_Object *ret;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();
	char trig;

	if (argc < 3) {
		InkWarn_Blueprint_For_Argument_Require(engine);
		return NULL_OBJ;
	}

	cond = argv[1];
	incr = argv[2];
	block = argc > 3 ? argv[3] : NULL;
	if (cond->type != INK_FUNCTION) {
		InkWarn_Blueprint_For_Require_Reference_Cond(engine);
		return NULL_OBJ;
	} else if (incr->type != INK_FUNCTION) {
		InkWarn_Blueprint_For_Require_Reference_Incr(engine);
		return NULL_OBJ;
	} else if (block && block->type != INK_FUNCTION) {
		InkWarn_Blueprint_For_Require_Block(engine);
		return NULL_OBJ;
	}

	ret = NULL;
	for (trig = 1; isTrue(cond->call(engine, context)); incr->call(engine, context), trig++) {
		if (!(trig % 5))
			gc_engine->checkGC();

		if (block) {
			ret = block->call(engine, context);
			if (engine->getSignal() != INTER_NONE) {
				switch (engine->getSignal()) {
					case INTER_RETURN:
						return engine->getInterruptValue(); // fallthrough the signal
					case INTER_DROP:
					case INTER_BREAK:
						return engine->trapSignal(); // trap the signal
					case INTER_CONTINUE:
						engine->trapSignal(); // trap the signal, but do not return
						continue;
					default:
						return NULL_OBJ;
				}
			}
		}
	}

	return ret ? ret : NULL_OBJ;
}

Ink_Object *InkMod_Blueprint_Base_Try(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *test_block = NULL;
	Ink_Object *catch_block = NULL;
	Ink_Object *final_block = NULL;
	Ink_Object *ret = NULL_OBJ;
	Ink_ArgcType i;
	Ink_Object **tmp_argv = NULL;
	Ink_InterruptSignal signal_back = INTER_NONE;
	Ink_ErrorMode err_mode_back = engine->getErrorMode();
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	if (!checkArgument(engine, argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}
	test_block = argv[0];

	for (i = 1; i < argc; i++) {
		if (argv[i]->type == INK_STRING) {
			if (as<Ink_String>(argv[i])->getWValue() == L"catch") {
				if (i + 1 < argc) {
					if (argv[i + 1]->type == INK_FUNCTION) {
						catch_block = argv[i + 1];
					} else {
						InkWarn_Blueprint_Try_Expect_Block_After_Catch(engine);
					}
				} else {
					InkWarn_Blueprint_Try_No_Argument_Follwing_Catch(engine);
				}
				i++;
			} else if (as<Ink_String>(argv[i])->getWValue() == L"final") {
				if (i + 1 < argc) {
					if (argv[i + 1]->type == INK_FUNCTION) {
						final_block = argv[i + 1];
					} else {
						InkWarn_Blueprint_Try_Expect_Block_After_Final(engine);
					}
				} else {
					InkWarn_Blueprint_Try_No_Argument_Follwing_Final(engine);
				}
				i++;
			} else {
				string tmp_str = as<Ink_String>(argv[i])->getValue();
				InkWarn_Blueprint_Try_Unknown_Instr(engine, tmp_str.c_str());
			}
		} else {
			InkWarn_Blueprint_Try_Unknown_Instr_Type(engine, argv[i]->type);
		}
	}

RETRY:
	engine->setErrorMode(INK_ERRMODE_STRICT);
	ret = test_block->call(engine, context);

	/* force collect garbage for some potential errors(e.g. illegal bonding) */
	engine->setGlobalReturnValue(ret);
	gc_engine->collectGarbage();
	
	engine->setErrorMode(err_mode_back);
	if (engine->getSignal() == INTER_THROW) {
		if (catch_block) {
			tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
			tmp_argv[0] = engine->getInterruptValue();

			engine->setInterrupt(INTER_NONE, NULL);
			ret = catch_block->call(engine, context, NULL, 1, tmp_argv);

			free(tmp_argv);
			if (engine->getSignal() == INTER_RETRY) {
				engine->trapSignal();
				// gc_engine->checkGC();
				goto RETRY;
			}
			if (engine->getSignal() != INTER_NONE) {
				signal_back = engine->getSignal();
				engine->setSignal(INTER_NONE);
				goto FINAL;
			}
		}
	} else {
		signal_back = engine->getSignal();
		engine->setSignal(INTER_NONE);
	}

FINAL:
	if (final_block) {
		final_block->call(engine, context);
	}
	engine->setSignal(signal_back);

	return ret;
}

void InkMod_Blueprint_Base_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	Ink_ParamList tmp_param_list;
	
	/* if */
	bondee->setSlot_c("if", new Ink_FunctionObject(engine, InkMod_Blueprint_Base_If, true));

	/* while */
	tmp_param_list = Ink_ParamList();
	tmp_param_list.push_back(Ink_Parameter(NULL, true));
	Ink_FunctionObject *while_func = new Ink_FunctionObject(engine, InkMod_Blueprint_Base_While, true);
	while_func->param = tmp_param_list;
	bondee->setSlot_c("while", while_func);

	/* for */
	tmp_param_list = Ink_ParamList();
	tmp_param_list.push_back(Ink_Parameter(NULL));
	tmp_param_list.push_back(Ink_Parameter(NULL, true));
	tmp_param_list.push_back(Ink_Parameter(NULL, true));
	Ink_FunctionObject *for_func = new Ink_FunctionObject(engine, InkMod_Blueprint_Base_For, true);
	for_func->param = tmp_param_list;
	bondee->setSlot_c("for", for_func);

	/* try */
	bondee->setSlot_c("try", new Ink_FunctionObject(engine, InkMod_Blueprint_Base_Try, true));

	return;
}

Ink_Object *InkMod_Blueprint_Base_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];

	InkMod_Blueprint_Base_bondTo(engine, apply_to);

	return NULL_OBJ;
}