/**
 * @file  rbf.h
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2006/12/29 02:09:00 $
 *    $Revision: 1.11 $
 *
 * Copyright (C) 2002-2007,
 * The General Hospital Corporation (Boston, MA). 
 * All rights reserved.
 *
 * Distribution, usage and copying of this software is covered under the
 * terms found in the License Agreement file named 'COPYING' found in the
 * FreeSurfer source code root directory, and duplicated here:
 * https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferOpenSourceLicense
 *
 * General inquiries: freesurfer@nmr.mgh.harvard.edu
 * Bug reports: analysis-bugs@nmr.mgh.harvard.edu
 *
 */


/*
 *       FILE NAME:   rbf.h
 *
 *       DESCRIPTION: prototypes and structures for Radial Basis Function
 *                    utilities.
 *
 *       AUTHOR:      Bruce Fischl
 *       DATE:        5/19/97
 *
*/

#ifndef RBF_H
#define RBF_H

/*-----------------------------------------------------
                    INCLUDE FILES
-------------------------------------------------------*/
#include "cluster.h"

#define MAX_OUTPUTS  30

typedef struct
{
  int          noutputs ;
  int          ninputs ;
  int          nhidden ;          /* number of RBFs */
  char         *class_names[MAX_OUTPUTS] ;
  CLUSTER_SET  *cs[MAX_OUTPUTS] ; /* one per class */
  float        *min_inputs ;      /* minimum feature values in input space */
  float        *max_inputs ;      /* maximum feature values in input space */
  MATRIX       *m_wij ;           /* weights for linear part of RBF */
  MATRIX       *m_delta_wij ;     /* delta weights for linear part of RBF */
  MATRIX       **m_delta_sigma_inv ;/* for use in momentum */
  VECTOR       **v_delta_means ;   /* for use in momentum */
  VECTOR       **v_z ;            /* zero-mean vectors */
  VECTOR       *v_outputs ;       /* outputs of the RBF */
  VECTOR       *v_hidden ;        /* activation of hidden layer */
  void         *parm ;            /* for clustering */
  int          (*get_observation_func)(VECTOR *v_obs, int no, void *parm);
  int          current_class ;    /* for clustering classes separately */
  float        momentum ;         /* current value of momentum */
  float        base_momentum ;
  float        trate ;            /* training rate */
  CLUSTER      **clusters ;       /* pointers to CLUSTER_SET clusters */
  unsigned char *observed ;       /* used for randomizing training order */
  int           nobs ;            /* # of observations in training set */
  MATRIX       *m_pi ;            /* psuedo-inverse for LMS calculation */
}
RBF ;

RBF   *RBFinit(int ninputs, int noutputs, int max_clusters[], char *names[]) ;
int   RBFtrain(RBF *rbf, int (*get_observation_func)
               (VECTOR *v_obs, int no, void *parm, int same_class,int *class),
               void *parm, float momentum) ;
int   RBFretrain(RBF *rbf, int (*get_observation_func)
                 (VECTOR *v_obs, int no, void *parm, int same_class,int *class),
                 void *parm, float momentum) ;
int   RBFfree(RBF **prbf) ;
int   RBFprint(RBF *rbf, FILE *fp) ;
int   RBFprintActivations(RBF *rbf, VECTOR *v_obs, VECTOR *v_error,
                          int class, FILE *fp);
int   RBFclassify(RBF *rbf, VECTOR *v_obs) ;
int   RBFwrite(RBF *rbf, char *fname) ;
RBF   *RBFread(char *fname) ;
int   RBFwriteInto(RBF *rbf, FILE *fp) ;
RBF   *RBFreadFrom(FILE *fp) ;
RBF   *RBFcopyWeights(RBF *rbf_src, RBF *rbf_dst) ;
float RBFcomputeErrors(RBF *rbf, int class, VECTOR *v_error) ;
int   RBFexamineTrainingSet(RBF *rbf, int (*get_observation_func)
                            (VECTOR *v_obs, int no, void *parm,
                             int same_class,int *pclass), void *parm) ;


#endif
