/**
 * @file
 * This file contains the declaration of Func_unique, which
 * is used to return a vector of unique values in a vector.
 *
 * @brief Declaration of Func_unique
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef Func_unique_H
#define Func_unique_H

#include "RbFunction.h"

#include <map>
#include <string>
#include <vector>

class DAGNode;
class VectorString;

const std::string Func_unique_name = "Unique function";

template <typename valType>
class Func_unique :  public RbFunction {
	
public:
	// Basic utility functions
	Func_unique*                clone(void) const;                                  //!< Clone the object
	const VectorString&         getClass(void) const;                               //!< Get class vector
    const TypeSpec&             getTypeSpec(void) const;                            //!< Get language type of the object
	
	// Regular functions
    const ArgumentRules&        getArgumentRules(void) const;                       //!< Get argument rules
	const TypeSpec&             getReturnType(void) const;                          //!< Get type of return value

protected:
	const RbLanguageObject&     executeFunction(void);                              //!< Execute operation
    
private:
    static const TypeSpec       typeSpec;
};

#endif

#include "Ellipsis.h"
#include "RbUtil.h"
#include "TypeSpec.h"
#include "ValueRule.h"


// Definition of the static type spec member
template <typename valType>
const TypeSpec Func_unique<valType>::typeSpec("Func_unique", new TypeSpec(valType().getType()));


/** Clone object */
template <typename valType>
Func_unique<valType>* Func_unique<valType>::clone( void ) const {
    
    return new Func_unique( *this );
}


/** Execute function: We rely on operator overloading to provide the necessary functionality */
template <typename valType> 
const RbLanguageObject& Func_unique<valType>::executeFunction( void ) {
    
    valType& val = static_cast<valType&>( (*args)[0].getValue() );
    
    if(val.size() == 0) 
        return val;
    val.sort();
    val.unique();
    return val;

}


/** Get argument rules */
template <typename valType>
const ArgumentRules& Func_unique<valType>::getArgumentRules( void ) const {
    
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
const VectorString& Func_unique<valType>::getClass( void ) const {
    
    static std::string  rbName  = "Func_unique<" + valType().getType() + ">"; 
    static VectorString rbClass = VectorString( rbName ) + RbFunction::getClass();
    
    return rbClass;
}


/** Get return type */
template <typename valType> 
const TypeSpec& Func_unique<valType>::getReturnType( void ) const {
	
    return valType().getTypeSpec();
}


/** Get return type */
template <typename valType>
const TypeSpec& Func_unique<valType>::getTypeSpec( void ) const {
    
    return typeSpec;
}


