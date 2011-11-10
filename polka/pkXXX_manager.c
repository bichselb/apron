/* ************************************************************************* */
/* internal.c: internal manager */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pkXXX_internal.h"
#include "ap_linconsXXX.h"
#include "ap_lingenXXX.h"

/* ********************************************************************** */
/* I. Constructor and destructor for internal */
/* ********************************************************************** */

/* Initialize pk with size maxdims */

void pkXXX_internal_init(pkXXX_internal_t* pk, size_t maxdims)
{
  size_t i;

  pk->exn = AP_EXC_NONE;

  pk->maxdims = maxdims;
  pk->maxcols = maxdims+3;

  pk->vector_numintp = vectorXXX_alloc(pk->maxcols);
  pk->vector_mpqp = malloc( (maxdims+3)*sizeof(mpq_t));
  for (i=0; i<maxdims+3; i++)
    mpq_init(pk->vector_mpqp[i]);

  pk->vector_tmp = vectorXXX_alloc(5);

  pk->matrix_dimp = malloc(pk->maxdims*sizeof(ap_dim_t));
  numintXXX_init(pk->matrix_acc);
  numintXXX_init(pk->matrix_prod);

  /* pk->cherni_bitstringp = bitstring_alloc(bitindex_size(pk->maxrows));*/
  pk->cherni_intp = (int*)malloc(pk->maxcols * sizeof(int));
  numintXXX_init(pk->cherni_prod);

  eitvXXX_init(pk->eitvXXX);
  ap_linexprMPQ_init(pk->ap_linexprMPQ,maxdims);
  ap_linconsMPQ_init(pk->ap_linconsMPQ,maxdims);
  ap_lingenXXX_init(pk->ap_lingenXXX,maxdims);
  ap_linexprMPQ_array_init(pk->linexprMPQ_array,maxdims);
  ap_linexprXXX_init(pk->envXXX,maxdims);
  for (i=0;i<maxdims;i++){
    pk->envXXX->linterm[i]->dim = i;
  }
  pk->envXXX->effsize = maxdims;
  numXXX_init(pk->numratXXX);
  numXXX_init(pk->numrat2XXX);
  numXXX_init(pk->numrat3XXX);
  pk->numintXXXp = vectorXXX_alloc(pk->maxcols);
  pk->numintXXXp2 = vectorXXX_alloc(pk->maxcols);
  pk->dimp = malloc(pk->maxdims*sizeof(ap_dim_t));
  pk->dimp2 = malloc(pk->maxdims*sizeof(ap_dim_t));
  /* pk->poly_bitstringp = bitstring_alloc(bitindex_size(pk->maxrows)); */
  pk->poly_matspecial = matrixXXX_alloc(1,pk->maxcols,true);
  numintXXX_init(pk->poly_prod);
}

/* Allocates pk and initializes it with a default size */
pkXXX_internal_t* pkXXX_internal_alloc(bool strict)
{
  pkXXX_internal_t* pk = (pkXXX_internal_t*)malloc(sizeof(pkXXX_internal_t));

  pk->strict = strict;
  pk->dec = strict ? 3 : 2;
  pk->option.max_coeff_size = 0;
  pk->option.approximate_max_coeff_size = 1;
  pk->option.expand_lazy = true;
  pk->option.forget_lazy = true;
  pk->option.remove_dimensions_lazy = false;
  pk->option.op_lazy = false;
  pk->option.strong_normalization = true;
  pk->option.widening_affine = true;

  pkXXX_internal_init(pk,10);

  return pk;
}

/* Clear pk */
void pkXXX_internal_clear(pkXXX_internal_t* pk)
{
  size_t i;

  if (pk->vector_numintp) vectorXXX_free(pk->vector_numintp,pk->maxcols);
  pk->vector_numintp = 0;

  if (pk->vector_tmp) vectorXXX_free(pk->vector_tmp,5);
  pk->vector_tmp = 0;

  if (pk->vector_mpqp){
    for (i=0; i<pk->maxdims+3; i++)
      mpq_clear(pk->vector_mpqp[i]);
    free(pk->vector_mpqp);
  }
  pk->vector_mpqp = 0;

  if (pk->matrix_dimp) free(pk->matrix_dimp);
  pk->matrix_dimp = 0;

  numintXXX_clear(pk->matrix_acc);
  numintXXX_clear(pk->matrix_prod);

  /*  if (pk->cherni_bitstringp) free(pk->cherni_bitstringp);
      pk->cherni_bitstringp = 0;
  */
  if (pk->cherni_intp) free(pk->cherni_intp);
  pk->cherni_intp = 0;

  numintXXX_clear(pk->cherni_prod);

  eitvXXX_clear(pk->eitvXXX);
  ap_linexprMPQ_clear(pk->ap_linexprMPQ);
  ap_linconsMPQ_clear(pk->ap_linconsMPQ);
  ap_lingenXXX_clear(pk->ap_lingenXXX);
  ap_linexprMPQ_array_clear(pk->linexprMPQ_array);
  ap_linexprXXX_clear(pk->envXXX);
  numXXX_clear(pk->numratXXX);
  numXXX_clear(pk->numrat2XXX);
  numXXX_clear(pk->numrat3XXX);
  if (pk->numintXXXp) vectorXXX_free(pk->numintXXXp, pk->maxcols);
  pk->numintXXXp = 0;

  if (pk->numintXXXp2) vectorXXX_free(pk->numintXXXp2, pk->maxcols);
  pk->numintXXXp2 = 0;

  if (pk->dimp) free(pk->dimp);
  pk->dimp = 0;
  if (pk->dimp2) free(pk->dimp2);
  pk->dimp2 = 0;

  /* if (pk->poly_bitstringp) bitstring_free(pk->poly_bitstringp);
     pk->poly_bitstringp = 0;
  */
  if (pk->poly_matspecial) matrixXXX_free(pk->poly_matspecial);
  pk->poly_matspecial = 0;

  numintXXX_clear(pk->poly_prod);

  pk->maxdims = 0;
  pk->maxrows = 0;
  pk->maxcols = 0;
}

/* Clear and free pk */
void pkXXX_internal_free(pkXXX_internal_t* pk)
{
  pkXXX_internal_clear(pk);
  free(pk);
}

/* Reallocate pk */
void pkXXX_internal_realloc(pkXXX_internal_t* pk, size_t maxdims)
{
  if (pk->maxdims != maxdims){
    pkXXX_internal_clear(pk);
    pkXXX_internal_init(pk,maxdims);
  }
}

/* Reallocate pk only if a bigger dimension is required */
void pkXXX_internal_realloc_lazy(pkXXX_internal_t* pk, size_t maxdims)
{
  if (maxdims > pk->maxdims){
    pkXXX_internal_clear(pk);
    pkXXX_internal_init(pk,maxdims);
  }
}

/* ********************************************************************** */
/* II. Options */
/* ********************************************************************** */

/* ********************************************************************** */
/* III. Initialization from manager */
/* ********************************************************************** */

ap_manager_t* pkXXX_manager_alloc(bool strict)
{
  size_t i;
  pkXXX_internal_t* pk;
  ap_manager_t* man;
  void** funptr;

  pk = pkXXX_internal_alloc(strict);
  man = ap_manager_alloc(strdup(strict ? "polka, strict mode, numXXX" : "polka, loose mode, numXXX"),
			 strdup("3.0"),
			 pk, (void (*)(void*))pkXXX_internal_free);
  pk->num = man->num;

  funptr = man->funptr;

  funptr[AP_FUNID_COPY] = &pkXXX_copy;
  funptr[AP_FUNID_FREE] = &pkXXX_free;
  funptr[AP_FUNID_ASIZE] = &pkXXX_size;
  funptr[AP_FUNID_MINIMIZE] = &pkXXX_minimize;
  funptr[AP_FUNID_CANONICALIZE] = &pkXXX_canonicalize;
  funptr[AP_FUNID_HASH] = &pkXXX_hash;
  funptr[AP_FUNID_APPROXIMATE] = &pkXXX_approximate;
  funptr[AP_FUNID_FPRINT] = &pkXXX_fprint;
  funptr[AP_FUNID_FPRINTDIFF] = &pkXXX_fprintdiff;
  funptr[AP_FUNID_FDUMP] = &pkXXX_fdump;
  funptr[AP_FUNID_SERIALIZE_RAW] = &pkXXX_serialize_raw;
  funptr[AP_FUNID_DESERIALIZE_RAW] = &pkXXX_deserialize_raw;
  funptr[AP_FUNID_BOTTOM] = &pkXXX_bottom;
  funptr[AP_FUNID_TOP] = &pkXXX_top;
  funptr[AP_FUNID_OF_BOX] = &pkXXX_of_box;
  funptr[AP_FUNID_DIMENSION] = &pkXXX_dimension;
  funptr[AP_FUNID_IS_BOTTOM] = &pkXXX_is_bottom;
  funptr[AP_FUNID_IS_TOP] = &pkXXX_is_top;
  funptr[AP_FUNID_IS_LEQ] = &pkXXX_is_leq;
  funptr[AP_FUNID_IS_EQ] = &pkXXX_is_eq;
  funptr[AP_FUNID_IS_DIMENSION_UNCONSTRAINED] = &pkXXX_is_dimension_unconstrained;
  funptr[AP_FUNID_SAT_INTERVAL] = &pkXXX_sat_interval;
  funptr[AP_FUNID_SAT_LINCONS] = &pkXXX_sat_lincons;
  funptr[AP_FUNID_SAT_TCONS] = &pkXXX_sat_tcons;
  funptr[AP_FUNID_BOUND_DIMENSION] = &pkXXX_bound_dimension;
  funptr[AP_FUNID_BOUND_LINEXPR] = &pkXXX_bound_linexpr;
  funptr[AP_FUNID_BOUND_TEXPR] = &pkXXX_bound_texpr;
  funptr[AP_FUNID_TO_BOX] = &pkXXX_to_box;
  funptr[AP_FUNID_TO_LINCONS_ARRAY] = &pkXXX_to_lincons_array;
  funptr[AP_FUNID_TO_TCONS_ARRAY] = &pkXXX_to_tcons_array;
  funptr[AP_FUNID_TO_LINGEN_ARRAY] = &pkXXX_to_lingen_array;
  funptr[AP_FUNID_MEET] = &pkXXX_meet;
  funptr[AP_FUNID_MEET_ARRAY] = &pkXXX_meet_array;
  funptr[AP_FUNID_MEET_LINCONS_ARRAY] = &pkXXX_meet_lincons_array;
  funptr[AP_FUNID_MEET_TCONS_ARRAY] = &pkXXX_meet_tcons_array;
  funptr[AP_FUNID_JOIN] = &pkXXX_join;
  funptr[AP_FUNID_JOIN_ARRAY] = &pkXXX_join_array;
  funptr[AP_FUNID_ADD_RAY_ARRAY] = &pkXXX_add_ray_array;
  funptr[AP_FUNID_ASSIGN_LINEXPR_ARRAY] = &pkXXX_assign_linexpr_array;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY] = &pkXXX_substitute_linexpr_array;
  funptr[AP_FUNID_ASSIGN_TEXPR_ARRAY] = &pkXXX_assign_texpr_array;
  funptr[AP_FUNID_SUBSTITUTE_TEXPR_ARRAY] = &pkXXX_substitute_texpr_array;
  funptr[AP_FUNID_ADD_DIMENSIONS] = &pkXXX_add_dimensions;
  funptr[AP_FUNID_REMOVE_DIMENSIONS] = &pkXXX_remove_dimensions;
  funptr[AP_FUNID_PERMUTE_DIMENSIONS] = &pkXXX_permute_dimensions;
  funptr[AP_FUNID_FORGET_ARRAY] = &pkXXX_forget_array;
  funptr[AP_FUNID_EXPAND] = &pkXXX_expand;
  funptr[AP_FUNID_FOLD] = &pkXXX_fold;
  funptr[AP_FUNID_WIDENING] = &pkXXX_widening;
  funptr[AP_FUNID_CLOSURE] = &pkXXX_closure;

  ap_manager_set_abort_if_exception(man, AP_EXC_TIMEOUT, false);
  ap_manager_set_abort_if_exception(man, AP_EXC_OUT_OF_SPACE, false);
  ap_manager_set_abort_if_exception(man, AP_EXC_OVERFLOW, false);

  return man;
}