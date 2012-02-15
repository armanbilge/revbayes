/**
 * @file
 * This file contains the declaration and implementation
 * of the templated Func__unot, which is used to perform
 * the unary RbBoolean not (!) operation.
 *
 * @brief Declaration and implementation of Func__unot
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 *
 * $Id$
 */

#ifndef Func__unot_H
#define Func__unot_H

#include "RbFunction.h"
#include "RbBoolean.h"

#include <string>

class DAGNode;
class VectorString;

template <typename valType>
class Func__unot :  public RbFunction {

    public:
        // Basic utility functions
        Func__unot*                 clone(void) const;                                          //!< Clone the object
        const VectorString&         getClass(void) const;                                       //!< Get class vector
        const TypeSpec&             getTypeSpec(void) const;                                    //!< Get language type of the object

        // Regular functions
        const ArgumentRules&        getArgumentRules(void) const;                               //!< Get argument rules
        const TypeSpec&             getReturnType(void) const;                                  //!< Get type of return value

    protected:
        const RbLanguageObject&     executeFunction(void);                                      //!< Execute function

    private:
        static const TypeSpec       typeSpec;
        static const TypeSpec       returnTypeSpec;
    
        // function return value
        RbBoolean                   retValue;
};

#endif

#include "RbBoolean.h"
#include "DAGNode.h"
#include "RbUtil.h"
#include "TypeSpec.h"
#include "ValueRule.h"
#include "VectorString.h"


// Definition of the static type spec member
template <typename valType>
const TypeSpec Func__unot<valType>::typeSpec("Func__unot", new TypeSpec(valType().getType()));
template <typename valType>
const TypeSpec Func__unot<valType>::returnTypeSpec(RbBoolean_name);


/** Clone object */
template <typename valType>
Func__unot<valType>* Func__unot<valType>::clone( void ) const {

    return new Func__unot( *this );
}


/** Execute function: We rely on operator overloading to provide the necessary functionality */
template <typename valType>
const RbLanguageObject& Func__unot<valType>::executeFunction( void ) {

    const valType& val = static_cast<valType&> ( (*args)[0].getValue() );
    retValue = ! (val);

    return retValue;
}


/** Get argument rules */
template <typename valType>
const ArgumentRules& Func__unot<valType>::getArgumentRules( void ) const {

    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;

    if ( !rulesSet ) 
        {
        argumentRules.push_back( new ValueRule( "", valType() .getTypeSpec() ) );
        rulesSet = true;
        }

    return argumentRules;
}


/** Get class vector describing type of object */
template <typename valType>
const VectorString& Func__unot<valType>::getClass( void ) const {

    static std::string  rbName  = "Func__unot<" + valType().getType() + ">"; 
    static VectorString rbClass = VectorString( rbName ) + RbFunction::getClass();
    
    return rbClass;
}


/** Get return type */
template <typename valType>
const TypeSpec& Func__unot<valType>::getReturnType( void ) const {

    return returnTypeSpec;
}


/** Get return spec */
template <typename valType>
const TypeSpec& Func__unot<valType>::getTypeSpec( void ) const {
    
    return typeSpec;
}

