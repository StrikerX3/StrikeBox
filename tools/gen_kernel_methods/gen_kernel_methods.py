#
# This is a script that was hacked together to parse the xboxkrnl.h and
# xboxkrnl.exe.def files to build a database of the Kernel import IDs and
# respective types. It uses libclang to parse the .h file and regex to parse the
# .def.
#
import sys
import re
from argparse import ArgumentParser

import os

# Add clang Python bindings
sys.path.append('pyclang')
from clang.cindex import *

def main():
    ap = ArgumentParser()
    ap.add_argument('--header_file')
    ap.add_argument('--def_file')
    ap.set_defaults(header_file='kdecl.h', def_file='xboxkrnl.exe.def')
    args = ap.parse_args()
    
    # Read .def file to get mapping of import IDs to var/func namne
    imports_id_to_str = {}
    with open(args.def_file, 'rb') as f:
        for line in f.readlines():
            m = re.match('\s*@?(\w+)(@\d+)?\s*@\s*(\d+).+', line)
            if not m: continue
            _id = int(m.group(3))
            name = m.group(1)
            if _id in imports_id_to_str:
                print "ID %d defined twice?" % (_id)
                assert(False)
            imports_id_to_str[_id] = name

    # Fill in the blanks with None
    max_import_id = max(imports_id_to_str)
    for i in range(max_import_id):
        if i not in imports_id_to_str:
            imports_id_to_str[i] = None

    # Build inverted map of name back ID for lookup
    imports_str_to_id = {}
    for _id, name in sorted(imports_id_to_str.items()):
        if name is not None and name in imports_str_to_id:
            print "Name %s defined twice?" % (name)
            assert(False)
        imports_str_to_id[name] = _id

    # Parse header file using libclang
    index = Index.create()
    tu = index.parse(args.header_file)

    # Get the first child of node that matches kind kind
    def get_first_child_by_kind(node, kind):
        for c in node.get_children():
            if c.kind is kind:
                return c

    # Get all children of this node of kind kind
    def filter_children_by_kind(node, kind):
        return [c for c in node.get_children() if c.kind is kind]

    # Get the type for this declaration node
    def get_type(node, pfx=''):
        return get_first_child_by_kind(node, CursorKind.TYPE_REF)

    # Get all variable declaration children of this node
    def get_var_declarations(node):
        return filter_children_by_kind(node, CursorKind.VAR_DECL)

    # Get all function declaration children of this node
    def get_function_declarations(node):
        return filter_children_by_kind(node, CursorKind.FUNCTION_DECL)

    # Get the calling convention for this function declaration
    # Note: I couldn't find a way for libclang to show the calling convention
    # in the AST so instead I mapped it to an annotation attribute so we can
    # pull it out. Maybe there's a better way to do it?
    def get_calling_convertion(node):
        return get_first_child_by_kind(node, CursorKind.ANNOTATE_ATTR)

    # Get the paramaters for this function declaration
    def get_params(node):
        return filter_children_by_kind(node, CursorKind.PARM_DECL)

    class FuncDecl(object):
        def __init__(self):
            self.import_id = -1
            self.calling_convention = 'stdcall'
            self.name = ''
            self.return_type = ''
            self.arguments = None

    class VarDecl(object):
        def __init__(self):
            self.import_id = -1
            self.name = ''
            self.type = ''

    func_decls = []
    var_decls = []

    # Extract function declarations
    for fcur in get_function_declarations(tu.cursor):      
        f = FuncDecl()  
        
        # Get function calling convention, return type, and arguments
        cc = get_calling_convertion(fcur)
        if cc: f.calling_convention = cc.spelling
        f.return_type = get_type(fcur).spelling
        f.name = fcur.spelling
        for n in get_params(fcur):
            assert(get_type(n).spelling)
            if n.spelling == '':
                print 'Unnamed argument for func', f.name
                assert(False)
        f.arguments = [(get_type(n).spelling, n.spelling) for n in get_params(fcur)]
        func_decls.append(f)

    # Extract variable declarations
    for vcur in get_var_declarations(tu.cursor):      
        v = VarDecl()
        v.type = get_type(vcur).spelling
        v.name = vcur.spelling
        var_decls.append(v)

    # Map function/variable declaration to import ID table
    i_table = [None for i in range(max(imports_id_to_str) + 1)]

    for x in (func_decls + var_decls):
        if x.name not in imports_str_to_id:
            print 'ERROR: Could not resolve %s in the .def file!' % (x.name)
            assert(False)
        _id = imports_str_to_id[x.name]
        x.import_id = imports_str_to_id[x.name]
        i_table[_id] = x

    # Print out all unpopulated entries in the import table
    unknown_import_ids = []
    for i, x in enumerate(i_table):
        if x is None:
            print 'WARNING: Definition of import ID %d is unknown!' % i
            unknown_import_ids.append(i)

    # Print out the extracted functions
    # print ''
    # print 'Functions:'
    # for f in sorted(func_decls, key=lambda x: x.import_id):
    #     params = ', '.join(['%s %s' % a for a in f.arguments])
    #     print '\t(Import %03d) %s %s %s(%s);' % (f.import_id, f.calling_convention, f.return_type, f.name, params)

    # Print out variable declarations
    # print ''
    # print 'Variables:'
    # for v in sorted(var_decls, key=lambda x: x.import_id):
    #     print '\t(Import %03d)' % v.import_id, v.type, v.name
    #     pass
    
    print ''
    print 'Found %d function declarations in header file' % len(func_decls)
    print 'Found %d variable declarations in header file' % len(var_decls)
    print 'Total = %d' % (len(func_decls) + len(var_decls))
    print 'Unknown import IDs:', unknown_import_ids

    os.makedirs('kernel')

    # Generate kernel variable definitions
    max_var_type_name_len = max([0] + map(lambda v: len(v.type), var_decls))

    with open('kernel/imports.h', 'wb') as f:
        # Generate list of imports
        f.write('#define KERNEL_IMPORTS \\\n')
        last = 0
        for v in sorted(var_decls + func_decls, key=lambda x:x.import_id):
            for i in range(last, v.import_id):
                f.write('\tKERNEL_IMPORT_NULL(%d) \\\n' % i)
            last = v.import_id + 1
            if isinstance(v, VarDecl):
                f.write('\tKERNEL_IMPORT_DATA(%d, %s) \\\n' % (v.import_id, v.name))
            elif isinstance(v, FuncDecl):
                f.write('\tKERNEL_IMPORT_FUNC(%d, %s) \\\n' % (v.import_id, v.name))
        f.write('\n\n')

        # Generate kernel variables
    with open('kernel/vars.h', 'wb') as f:
        f.write('#include "common.h"\n\n')
        f.write('struct KernelVariables {\n')
        for v in sorted(var_decls, key=lambda x:x.import_id):
            f.write('\t%s %s;\n' % (v.type.ljust(max_var_type_name_len), v.name))
        f.write('};\n\n')

        # # Generate kernel function prototypes
        # for func in func_decls:
        #     f.write('int Kernel%s(CallingConventionHelper &cc);\n' % func.name)

    # Generate function definitions
    with open('kernel/common.h', 'wb') as f:
        f.write('#include "types.h"')

    for func in sorted(func_decls, key=lambda x:x.name):
        with open('kernel/%s.cpp' % func.name, 'wb') as f:
            f.write('#include "common.h"\n\n')

            # Generate Comment
            f.write('/*\n')
            f.write(' * %s\n' % func.name)
            f.write(' *\n')
            f.write(' * Import Number:      %d\n' % func.import_id)
            f.write(' * Calling Convention: %s\n' % func.calling_convention)

            max_param_type_name_len = max([0] + map(lambda x: len(x[0]), func.arguments))

            for i, param in enumerate(func.arguments):
                type_name = param[0].ljust(max_param_type_name_len)
                f.write(' * Parameter %d:        %s %s\n' % (i, type_name, param[1]))
            f.write(' * Return Type:        %s\n' % func.return_type)
            f.write(' */\n')

            # Generate Function Definitions
            f.write('int Xbox::%s()\n' % func.name)
            f.write('{\n')
            f.write('\tK_ENTER_%s();\n' % func.calling_convention.upper())

            for i, param in enumerate(func.arguments):
                f.write('\tK_INIT_ARG(%s %s);\n' % ((param[0] + ',').ljust(max_param_type_name_len+1), param[1]))

            if func.return_type.lower() != 'void':
                f.write('\t%s rval;\n' % func.return_type)

            f.write('\n')

            if func.return_type.lower() != 'void':
                f.write('\tK_EXIT_WITH_VALUE(rval);\n')
            else:
                f.write('\tK_EXIT();\n')

            f.write('\treturn ERROR_NOT_IMPLEMENTED;\n')
            f.write('}\n')

if __name__ == '__main__':
    main()
