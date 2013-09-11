/*
 * Copyright (c) 2013  University of Texas at Austin. All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * This file is part of PerfExpert.
 *
 * PerfExpert is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * PerfExpert is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PerfExpert. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Leonardo Fialho
 *
 * $HEADER$
 */

#ifdef __cplusplus
extern "C" {
#endif

/* System standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Utility headers */
#include <sqlite3.h>

/* PerfExpert headers */
#include "config.h"
#include "ct.h"
#include "perfexpert_constants.h"
#include "perfexpert_output.h"
#include "perfexpert_list.h"

/* select_transformations */
int select_transformations(fragment_t *fragment) {
    recommendation_t *recommendation;
    char sql[BUFFER_SIZE];
    char temp_str[BUFFER_SIZE];
    char *error_msg = NULL;

    OUTPUT_VERBOSE((7, "=== %s", _BLUE("Querying DB")));

    /* For each recommendation in this fragment, select the transformers */
    recommendation = (recommendation_t *)perfexpert_list_get_first(
        &(fragment->recommendations));

    while ((perfexpert_list_item_t *)recommendation !=
        &(fragment->recommendations.sentinel)) {
        OUTPUT_VERBOSE((4, "   %s [%d]",
            _YELLOW("selecting transformations for recommendation"),
            recommendation->id));

        /* Initialyze the list of transformations */
        perfexpert_list_construct(&(recommendation->transformations));

        /* Find the transformations available for this recommendation */
        bzero(sql, BUFFER_SIZE);
        strcat(sql, "SELECT t.transformer, t.id FROM transformation AS t ");
        strcat(sql, "INNER JOIN recommendation_transformation AS rt ");
        strcat(sql, "ON t.id = rt.id_transformation ");
        bzero(temp_str, BUFFER_SIZE);
        sprintf(temp_str, "WHERE rt.id_recommendation = %d;",
            recommendation->id);
        strcat(sql, temp_str);
        OUTPUT_VERBOSE((10, "      SQL: %s", _CYAN(sql)));

        if (SQLITE_OK != sqlite3_exec(globals.db, sql,
            accumulate_transformations, (void *)recommendation, &error_msg)) {
            fprintf(stderr, "Error: SQL error: %s\n", error_msg);
            sqlite3_free(error_msg);
            return PERFEXPERT_ERROR;
        }

        /* Move to the next code recommendation */
        recommendation = (recommendation_t *)perfexpert_list_get_next(
            (perfexpert_list_item_t *)recommendation);
    }
    return PERFEXPERT_SUCCESS;
}

/* accumulate_transformations */
int accumulate_transformations(void *recommendation, int count, char **val,
    char **names) {
    recommendation_t *r;
    transformation_t *transformation;
    char sql[BUFFER_SIZE];
    char temp_str[BUFFER_SIZE];
    char *error_msg = NULL;

    /* Create the transformation item */
    transformation = (transformation_t *)malloc(sizeof(transformation_t));
    if (NULL == transformation) {
        OUTPUT(("%s", _ERROR("Error: out of memory")));
        return PERFEXPERT_ERROR;
    }
    perfexpert_list_item_construct((perfexpert_list_item_t *)transformation);

    /* Set the transformation ID */
    transformation->id = atoi(val[1]);

    /* Set the transformer, aka program */
    transformation->program = (char *)malloc(strlen(val[0]) + 1);
    bzero(transformation->program, strlen(val[0]) + 1);
    strncpy(transformation->program, val[0], strlen(val[0]));

    OUTPUT_VERBOSE((7, "      %s [%d]", _GREEN(transformation->program),
        transformation->id));

    OUTPUT_VERBOSE((4, "      %s [%d]",
        _YELLOW("selecting patterns for transformation"), transformation->id));

    /* Initialize the list of pattern recognizers */
    perfexpert_list_construct(&(transformation->patterns));

    /* Find the pattern recognizers available for this transformation */
    bzero(sql, BUFFER_SIZE);
    strcat(sql, "SELECT p.recognizer, p.id FROM pattern AS p ");
    strcat(sql, "INNER JOIN transformation_pattern AS tp ");
    strcat(sql, "ON p.id = tp.id_pattern ");
    bzero(temp_str, BUFFER_SIZE);
    sprintf(temp_str, "WHERE tp.id_transformation = %d;", transformation->id);
    strcat(sql, temp_str);
    OUTPUT_VERBOSE((10, "         SQL: %s", _CYAN(sql)));

    if (SQLITE_OK != sqlite3_exec(globals.db, sql, accumulate_patterns,
        (void *)transformation, &error_msg)) {
        fprintf(stderr, "Error: SQL error: %s\n", error_msg);
        sqlite3_free(error_msg);
        return PERFEXPERT_ERROR;
    }

    /* Add this transformation to the recommendation list */
    r = (recommendation_t *)recommendation;
    perfexpert_list_append((perfexpert_list_t *)&(r->transformations),
        (perfexpert_list_item_t *)transformation);

    /* TODO: Free memory */

    return PERFEXPERT_SUCCESS;
}

/* accumulate_patterns */
int accumulate_patterns(void *transformation, int count, char **val,
    char **names) {
    transformation_t *t;
    pattern_t *pattern;

    /* Create the pattern item */
    pattern = (pattern_t *)malloc(sizeof(pattern_t));
    if (NULL == pattern) {
        OUTPUT(("%s", _ERROR("Error: out of memory")));
        return PERFEXPERT_ERROR;
    }
    perfexpert_list_item_construct((perfexpert_list_item_t *)pattern);

    /* Set the pattern ID */
    pattern->id = atoi(val[1]);

    /* Set the recognizer, aka program */
    pattern->program = (char *)malloc(strlen(val[0]) + 1);
    bzero(pattern->program, strlen(val[0]) + 1);
    strncpy(pattern->program, val[0], strlen(val[0]));

    OUTPUT_VERBOSE((7, "         %s (%d)", _GREEN(pattern->program),
        pattern->id));

    /* Add this pattern to the transformation list */
    t = (transformation_t *)transformation;
    perfexpert_list_append((perfexpert_list_t *)&(t->patterns),
        (perfexpert_list_item_t *)pattern);

    /* TODO: Free memory */

    return PERFEXPERT_SUCCESS;
}

#ifdef __cplusplus
}
#endif

// EOF