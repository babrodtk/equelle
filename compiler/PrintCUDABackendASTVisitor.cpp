/*
  Copyright 2013 SINTEF ICT, Applied Mathematics.
*/

#include "PrintCUDABackendASTVisitor.hpp"
#include <iostream>
#include <string>

namespace
{
    const char* impl_cppStartString();
    const char* impl_cppEndString();
}



PrintCUDABackendASTVisitor::PrintCUDABackendASTVisitor()
{
}

PrintCUDABackendASTVisitor::~PrintCUDABackendASTVisitor()
{
}

const char *PrintCUDABackendASTVisitor::cppStartString() const
{
    return 
"\n"
"// This program was created by the Equelle compiler from SINTEF.\n"
"\n"
"#include <opm/core/utility/parameters/ParameterGroup.hpp>\n"
"#include <opm/core/utility/ErrorMacros.hpp>\n"
"#include <opm/core/grid.h>\n"
"#include <opm/core/grid/GridManager.hpp>\n"
"#include <algorithm>\n"
"#include <iterator>\n"
"#include <iostream>\n"
"#include <cmath>\n"
"#include <array>\n"
"\n"
"#include \"EquelleRuntimeCUDA.hpp\"\n"
"\n"
"void ensureRequirements(const EquelleRuntimeCUDA& er);\n"
"void equelleGeneratedCode(equelleCUDA::EquelleRuntimeCUDA& er);\n"
"\n"
"#ifndef EQUELLE_NO_MAIN\n"
"int main(int argc, char** argv)\n"
"{\n"
"    // Get user parameters.\n"
"    Opm::parameter::ParameterGroup param(argc, argv, false);\n"
"\n"
"    // Create the Equelle runtime.\n"
"    equelleCUDA::EquelleRuntimeCUDA er(param);\n"
"    equelleGeneratedCode(er);\n"
"    return 0;\n"
"}\n"
"#endif // EQUELLE_NO_MAIN\n"
"\n"
"void equelleGeneratedCode(equelleCUDA::EquelleRuntimeCUDA& er) {\n"
"    using namespace equelleCUDA;\n"
"    ensureRequirements(er);\n"
"\n"
"    // ============= Generated code starts here ================\n";
}

const char *PrintCUDABackendASTVisitor::cppEndString() const
{
    return
"    // ============= Generated code ends here ================\n"
"\n"
"}\n";
}

const char *PrintCUDABackendASTVisitor::classNameString() const
{
    return "EquelleRuntimeCUDA";
}

const char *PrintCUDABackendASTVisitor::namespaceNameString() const
{
    return "equelleCUDA";
}

