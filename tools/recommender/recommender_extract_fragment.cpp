/*
 * Copyright (c) 2013  University of Texas at Austin. All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * This file is part of OptTran and PerfExpert.
 *
 * OptTran as well PerfExpert are free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * OptTran and PerfExpert are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OptTran or PerfExpert. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Leonardo Fialho
 *
 * $HEADER$
 */

/* System standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Utility headers */
#include <rose.h>
#include <sage3.h>

/* OptTran headers */
#include "config.h"
#include "recommender.h"
#include "opttran_output.h"
#include "opttran_util.h"
#include "recommenderTraversal.h"

extern globals_t globals; // globals was defined on 'recommender.c'

int extract_fragment(segment_t *segment) {
    recommenderTraversal segmentTraversal;
    SgProject *userProject = NULL;
    char **files = NULL;
    char *fragments_dir = NULL;
    int filenum;
    int i;

    OPTTRAN_OUTPUT_VERBOSE((7, "=== %s (%s:%d) [%s]",
                            _BLUE((char *)"Extracting fragment from"),
                            segment->filename, segment->line_number,
                            segment->type));
    
    /* Fill 'files', aka **argv */
    files = (char **)malloc(sizeof(char *) * 3);
    files[0] = (char *)malloc(sizeof("recommender") + 1);
    if (NULL == files[0]) {
        OPTTRAN_OUTPUT(("%s", _ERROR((char *)"Error: out of memory")));
        exit(OPTTRAN_ERROR);
    }
    bzero(files[0], sizeof("recommender") + 1);
    snprintf(files[0], sizeof("recommender"), "recommender");
    files[1] = globals.source_file;
    files[2] = NULL;

    /* Load files and build AST */
    userProject = frontend(2, files);
    ROSE_ASSERT(userProject != NULL);

    /* Build the traversal object and call the traversal function starting at
     * the project node of the AST, using a pre-order traversal
     */
    fragments_dir = (char *)malloc(strlen(globals.opttrandir) +
                                   strlen(OPTTRAN_FRAGMENTS_DIR) + 10);
    if (NULL == fragments_dir) {
        OPTTRAN_OUTPUT(("%s", _ERROR((char *)"Error: out of memory")));
        exit(OPTTRAN_ERROR);
    }
    bzero(fragments_dir, (strlen(globals.opttrandir) +
                          strlen(OPTTRAN_FRAGMENTS_DIR) + 10));
    sprintf(fragments_dir, "%s/%s", globals.opttrandir, OPTTRAN_FRAGMENTS_DIR);
    if (OPTTRAN_ERROR == opttran_util_make_path(fragments_dir, 0755)) {
        OPTTRAN_OUTPUT(("%s", _ERROR((char *)"Error: cannot create fragments directory")));
        free(fragments_dir);
        exit(OPTTRAN_ERROR);
    } else {
        OPTTRAN_OUTPUT_VERBOSE((8, "...to (%s)", fragments_dir));
        free(fragments_dir);
    }
    segmentTraversal.item = segment;
    segmentTraversal.traverseInputFiles(userProject, preorder);

    /* Insert manipulations of the AST here... */

    /* Generate source code output on OPTTRAN directory */
    // TODO: generate files in the right place
    filenum = userProject->numberOfFiles();
    for (i = 0; i < filenum; ++i) {
        SgSourceFile* file = isSgSourceFile(userProject->get_fileList()[i]);
        file->unparse();
    }

    /* I believe now it is OK to free 'argv' */
    free(files[0]);
    free(files);

    OPTTRAN_OUTPUT_VERBOSE((7, "%s",
                            _GREEN((char *)"fragment extraction concluded")));
    
    return OPTTRAN_SUCCESS;
}

// EOF
