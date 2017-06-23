cmake_minimum_required (VERSION 2.8.11)
project (wv17)

set(VAL_SOURCES
	src/VAL/DebugWriteController.cpp
	src/VAL/FastEnvironment.cpp
	src/VAL/FuncAnalysis.cpp
	src/VAL/SASActions.cpp
	src/VAL/SimpleEval.cpp
	src/VAL/TIM.cpp
	src/VAL/TimSupport.cpp
	src/VAL/ToFunction.cpp
	src/VAL/TypedAnalyser.cpp
	src/VAL/instantiation.cpp
	src/VAL/pddl+.cpp
	src/VAL/ptree.cpp
	src/VAL/typecheck.cpp
	src/VAL/Validator.cpp
	src/VAL/Action.cpp
	src/VAL/Proposition.cpp
	src/VAL/FuncExp.cpp
	src/VAL/RepairAdvice.cpp
	src/VAL/State.cpp
	src/VAL/Plan.cpp
	src/VAL/Ownership.cpp
	src/VAL/Environment.cpp
	src/VAL/Events.cpp
	src/VAL/Polynomial.cpp
	src/VAL/TrajectoryConstraints.cpp
	src/VAL/RobustAnalyse.cpp
	src/VAL/PrettyPrinter.cpp
	src/VAL/random.cpp
	src/VAL/LaTeXSupport.cpp
	src/VAL/Utils.cpp)

include_directories(include/VAL)

add_library (parser ${VAL_SOURCES})

add_library(ourParser src/ourParser.cpp)

add_executable (parser_test src/parser_test.cpp)

target_link_libraries (parser_test LINK_PUBLIC parser)

add_executable (csp_test src/csp_test.cpp)

add_executable(solver src/solver.cpp)

target_link_libraries (solver LINK_PUBLIC ourParser parser)

target_link_libraries (csp_test LINK_PUBLIC gecodeint gecodekernel gecodesupport gecodesearch gecodedriver gecodeminimodel)
