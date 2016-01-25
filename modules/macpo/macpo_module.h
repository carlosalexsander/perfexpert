/*
 * Copyright (c) 2011-2016  University of Texas at Austin. All rights reserved.
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

#ifndef PREFEXPERT_MODULE_MACPO_MODULE_H_
#define PREFEXPERT_MODULE_MACPO_MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Modules headers */
#include "modules/perfexpert_module_base.h"
#include "modules/perfexpert_module_measurement.h"

typedef struct{
    perfexpert_module_measurement_t *measurement;
    perfexpert_module_1_0_0_t macpo_module;
} perfexpert_macpo_module_1_0_0_t;

perfexpert_macpo_module_1_0_0_t myself_module;

//perfexpert_module_1_0_0_t myself_module;

#ifdef __cplusplus
}
#endif

#endif /* PREFEXPERT_MODULE_MACPO_MODULE_H_ */
