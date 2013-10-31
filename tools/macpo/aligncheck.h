/*
 * Copyright (c) 2011-2013  University of Texas at Austin. All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * This file is part of PerfExpert.
 *
 * PerfExpert is free software: you can redistribute it and/or modify it under
 * the terms of the The University of Texas at Austin Research License
 * 
 * PerfExpert is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.
 * 
 * Authors: Leonardo Fialho and Ashay Rane
 *
 * $HEADER$
 */

#ifndef	ALIGNCHEK_H_
#define	ALIGNCHEK_H_

#include <VariableRenaming.h>

#include "generic_defs.h"

class aligncheck_t : public AstTopDownProcessing<attrib> {

    enum { LOOP_TEST=0, LOOP_INCR, LOOP_INIT, BASE_ALIGNMENT };

    typedef VariableRenaming::NumNodeRenameEntry::iterator entry_iterator;
    typedef std::map<std::string, VariableRenaming::NumNodeRenameEntry>
            def_map_t;

    public:
    aligncheck_t(VariableRenaming*& _var_renaming);

    bool vectorizable(SgStatement*& stmt);

    virtual void atTraversalEnd();
    virtual void atTraversalStart();

    virtual attrib evaluateInheritedAttribute(SgNode* node, attrib attr);

    bool get_loop_header_components(SgForStatement*& for_stmt,
            def_map_t& def_map, SgExpression*& idxv_expr,
            SgExpression*& init_expr, SgExpression*& test_expr,
            SgExpression*& incr_expr);

    const inst_list_t::iterator inst_begin();
    const inst_list_t::iterator inst_end();

    private:
    SgExpression* get_expr_value(SgNode*& node, std::string var_name);

    void construct_def_map(VariableRenaming::NumNodeRenameTable& rename_table,
        std::map<std::string, VariableRenaming::NumNodeRenameEntry>& def_map);

    void instrument_loop_header_components(SgNode*& loop_node,
            def_map_t& def_map, SgExpression*& instrument_expr,
            short loop_inst_type);

    void insert_instrumentation(SgNode*& node, SgExpression*& expr,
            short type, bool before);

    VariableRenaming* var_renaming;
    inst_list_t inst_info_list;
};

#endif	/* ALIGNCHEK_H_ */