/**
 * @file
 * This file contains the declaration of RevLanguage wrapper of VectorSingleElementScaleMove.
 *
 * @brief Declaration of Move_ScaleSingleACLNRates
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date: 2012-08-06 20:14:22 +0200 (Mon, 06 Aug 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-11-20, version 1.0
 * @extends RbObject
 *
 */

#ifndef Move_ScaleSingleACLNRates_H
#define Move_ScaleSingleACLNRates_H

#include "SimpleMove.h"
#include "RlMove.h"
#include "TypedDagNode.h"

#include <ostream>
#include <string>

namespace RevLanguage {
    
    class Move_ScaleSingleACLNRates : public Move {
        
    public:
        
        Move_ScaleSingleACLNRates(void);                                                                                      //!< Default constructor (0.0)
        
        // Basic utility functions
        virtual Move_ScaleSingleACLNRates*       clone(void) const;                                                      //!< Clone object
        void                                        constructInternalObject(void);                                          //!< We construct the a new internal SlidingMove.
        static const std::string&                   getClassType(void);                                                     //!< Get Rev type
        static const TypeSpec&                      getClassTypeSpec(void);                                                 //!< Get class type spec
        const MemberRules&                          getMemberRules(void) const;                                             //!< Get member rules (const)
        virtual const TypeSpec&                     getTypeSpec(void) const;                                                //!< Get language type of the object
        virtual void                                printValue(std::ostream& o) const;                                      //!< Print value (for user)
        
    protected:
        
        void                                        setConstMemberVariable(const std::string& name, const RevPtr<const Variable> &var);     //!< Set member variable
        
        RevPtr<const Variable>                      v;
        RevPtr<const Variable>                      lambda;
        RevPtr<const Variable>                      weight;
        RevPtr<const Variable>                      tune;
        
    };
    
}

#endif