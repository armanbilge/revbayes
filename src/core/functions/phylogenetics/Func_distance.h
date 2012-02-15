/**
 * @file
 * This file contains the declaration of Func_distance, which is used to construct
 * a rate matrix for the GTR model of nucleotide substitution
 * 
 * @brief Declaration of Func_distance
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef Func_distance_H
#define Func_distance_H

#include "RbFunction.h"
#include "DistanceMatrix.h"

class DAGNode;
class VectorString;



const std::string Func_distance_name = "Distance matrix function";

class Func_distance :  public RbFunction {
    
    public:
    Func_distance(void);
    
        // Basic utility functions
        Func_distance*              clone(void) const;                                                       //!< Clone the object
        const VectorString&         getClass(void) const;                                                    //!< Get class vector
        const TypeSpec&             getTypeSpec(void) const;                                                 //!< Get language type of the object
        
        // Regular functions
        const ArgumentRules&        getArgumentRules(void) const;                                            //!< Get argument rules
        const TypeSpec&             getReturnType(void) const;                                               //!< Get type of return value

    protected:
        const RbLanguageObject&     executeFunction(void);                                                   //!< Execute function

    private:
        static const TypeSpec       typeSpec;
        static const TypeSpec       returnTypeSpec;
        double                      distanceP(const TaxonData& td1, const TaxonData& td2);
        double                      distanceJC69(const TaxonData& td1, const TaxonData& td2);

        // function return value
        DistanceMatrix              matrix;

};

#endif

