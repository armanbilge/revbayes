/**
 * @file
 * This file contains the declaration of Dist_ctmm, which holds
 * the parameters and functions related to a distribution on
 * discrete states from a continuous-time discrete-state Markov
 * model.
 *
 * @brief Declaration of Dist_ctmm
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (M�n, 14 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberObject.h 194 2009-12-14 11:43:32Z ronquist $
 */

#ifndef Dist_ctmm_H
#define Dist_ctmm_H

#include "CharacterStateDiscrete.h"
#include "DistributionDiscrete.h"

#include <ostream>
#include <string>

class RateMatrix;
class VectorString;

class Dist_ctmm: public DistributionDiscrete {

    public:
                                    Dist_ctmm(void);                                                    //!< Parser constructor
                                    Dist_ctmm(RateMatrix* Q, double v, CharacterStateDiscrete* a);      //!< Internal constructor

        // Basic utility functions
        Dist_ctmm*                  clone(void) const;                                                  //!< Clone object
        const VectorString&         getClass(void) const;                                               //!< Get class vector

        // Member variable setup
        const MemberRules&          getMemberRules(void) const;                                         //!< Get member variable rules
        void                        setMemberVariable(const std::string& name, Variable* var);          //!< Set member variable (ensure number of states is consistent)

        // Discrete distribution functions
        size_t                      getNumberOfStates(void) const;                                      //!< Get number of states
        virtual const Simplex*      getProbabilityMassVector(void);                                     //!< Get probability mass vector
        const TypeSpec              getVariableType(void) const;                                        //!< Get random variable type (Simplex)
        double                      lnPdf(const RbLanguageObject* value);                               //!< Ln probability density
        double                      pdf(const RbLanguageObject* value);                                 //!< Probability density
        CharacterStateDiscrete*     rv(void);                                                           //!< Generate random variable
};

#endif

