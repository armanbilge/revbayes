/**
 * @file
 * This file contains the declaration of Dist_unif, which is used to hold
 * parameters and functions related to a uniform distribution.
 *
 * @brief Declaration of Dist_unif
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (Mån, 14 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberObject.h 194 2009-12-14 11:43:32Z ronquist $
 */

#ifndef Dist_unif_H
#define Dist_unif_H

#include "DistributionContinuous.h"
#include "Real.h"

#include <ostream>
#include <string>

class VectorString;

class Dist_unif: public DistributionContinuous {

    public:
                                    Dist_unif(void);                                                    //!< Parser constructor
                                    Dist_unif(double min, double max);                                  //!< Internal constructor

        // Basic utility functions
        Dist_unif*                  clone(void) const;                                                  //!< Clone object
        const VectorString&         getClass(void) const;                                               //!< Get class vector

        // Member variable setup
        const MemberRules&          getMemberRules(void) const;                                         //!< Get member variable rules

        // Real-valued distribution functions
        double                      cdf(const RbLanguageObject* value);                                 //!< Cumulative density
        const Real*                 getMax(void) const;                                                 //!< Get max value
        const Real*                 getMin(void) const;                                                 //!< Get min value
        const TypeSpec              getVariableType(void) const;                                        //!< Get random variable type (Real)
        double                      lnPdf(const RbLanguageObject* value);                               //!< Ln probability density
        double                      pdf(const RbLanguageObject* value);                                 //!< Probability density
        Real*                       quantile(const double p);                                           //!< Quantile
        Real*                       rv(void);                                                           //!< Generate random variable
};

#endif

