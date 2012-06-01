/**
 * @file
 * This file contains the declaration of Func_sort, which
 * is used to return a sorted vector from an input vector.
 *
 * @brief Declaration of Func_sort
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef Func_sort_H
#define Func_sort_H

#include "RbFunction.h"

#include <map>
#include <string>
#include <vector>

class DAGNode;

const std::string Func_sort_name = "Sort function";

template <typename valType>
class Func_sort :  public RbFunction {
	
public:
	// Basic utility functions
	Func_sort*                  clone(void) const;                                  //!< Clone the object
    static const std::string&   getClassName(void);                                 //!< Get class name
    static const TypeSpec&      getClassTypeSpec(void);                             //!< Get class type spec
    const TypeSpec&             getTypeSpec(void) const;                            //!< Get language type of the object
	
	// Regular functions
    const ArgumentRules&        getArgumentRules(void) const;                       //!< Get argument rules
	const TypeSpec&             getReturnType(void) const;                          //!< Get type of return value

protected:
    RbPtr<RbLanguageObject>     executeFunction(void);                              //!< Execute operation

};

#endif

#include "Ellipsis.h"
#include "RbUtil.h"
#include "TypeSpec.h"



/** Clone object */
template <typename valType>
Func_sort<valType>* Func_sort<valType>::clone( void ) const {
    
    return new Func_sort( *this );
}


/** Execute function: We rely on operator overloading to provide the necessary functionality */
template <typename valType> 
RbPtr<RbLanguageObject> Func_sort<valType>::executeFunction( void ) {
    
    valType *retValue = static_cast<valType *>( args[0]->getVariable()->getValue().clone() );    
    if(retValue->size() == 0) 
        return retValue;
    retValue->sort();
    
    return retValue;
}


/** Get argument rules */
template <typename valType>
const ArgumentRules& Func_sort<valType>::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet ) 
    {
        argumentRules.push_back( new ArgumentRule( "value", true, valType::getClassTypeSpec() ) );
        rulesSet = true;
    }
    
    return argumentRules;
}


/** Get class name of object */
template <typename firstValType>
const std::string& Func_sort<firstValType>::getClassName(void) { 
    
    static std::string rbClassName = "Func_sort<" + firstValType().getTypeSpec() + ">";
    
	return rbClassName; 
}


/** Get class type spec describing type of object */
template <typename firstValType>
const TypeSpec& Func_sort<firstValType>::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( RbFunction::getClassTypeSpec() ) );
    
	return rbClass; 
}


/** Get type spec */
template <typename firstValType>
const TypeSpec& Func_sort<firstValType>::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get return type */
template <typename valType> 
const TypeSpec& Func_sort<valType>::getReturnType( void ) const {
	
    static TypeSpec retType = valType().getTypeSpec();
    return retType;
}


