/**
 * @file
 * This file contains the declaration of DistributionFunction, which
 * is used for functions related to a statistical distribution.
 *
 * @brief Declaration of DistributionFunction
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-05-10 16:04:22 +0200 (Thu, 10 May 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id: DistributionFunction.h 1529 2012-05-10 14:04:22Z hoehna $
 */

#ifndef DistributionFunctionCdf_H
#define DistributionFunctionCdf_H

#include "RbFunction.h"

#include <string>
#include <vector>

class ParserDistributionContinuous;


class DistributionFunctionCdf :  public RbFunction {
    
public:
    
    DistributionFunctionCdf(const RbPtr<ParserDistributionContinuous> &dist);                                       //!< Constructor
    DistributionFunctionCdf(const DistributionFunctionCdf& x);                                                      //!< Copy constructor
    
    // Assignment operator
    DistributionFunctionCdf&                operator=(const DistributionFunctionCdf& x);                            //!< Assignment operator
    
    // Basic utility functions
    DistributionFunctionCdf*                clone(void) const;                                                      //!< Clone object
    static const std::string&               getClassName(void);                                                     //!< Get class name
    static const TypeSpec&                  getClassTypeSpec(void);                                                 //!< Get class type spec
    const TypeSpec&                         getTypeSpec(void) const;                                                //!< Get language type of the object
    
    // DistributionFunction functions
    const ArgumentRules&                    getArgumentRules(void) const;                                           //!< Get argument rules
    const TypeSpec&                         getReturnType(void) const;                                              //!< Get type spec of return value
    void                                    processArguments(const std::vector<Argument>& passedArgs);              //!< Process args, set member variables of distribution
    
protected:
    RbPtr<RbLanguageObject>                 executeFunction(const std::vector<const RbObject*>& args);              //!< Execute function
    
    RbPtr<ArgumentRules>                    argumentRules;                                                          //!< Argument rules
    TypeSpec                                returnType;                                                             //!< Return type
    RbPtr<ParserDistributionContinuous>     distribution;                                                           //!< The distribution
    
};

#endif

