/**
 * @file
 * This file contains the implementation of a Plate.
 *
 * @brief Implementation of Plate
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-03-23 19:18:46 +0100 (Fri, 23 Mar 2012) $
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-12-04, version 1.0
 * @extends MemberObject
 *
 * $Id: Plate.cpp 1360 2012-03-23 18:18:46Z hoehna $
 */

#include "ConstantNode.h"
#include "ConstArgumentRule.h"
#include "DeterministicNode.h"
#include "Distribution.h"
#include "MethodTable.h"
#include "Ellipsis.h"
#include "OptionRule.h"
#include "ParserDistribution.h"
#include "Plate.h"
#include "RbNullObject.h"
#include "RbUtil.h"
#include "StochasticNode.h"
#include "Workspace.h"


/* Default constructor */
Plate::Plate(void) : MemberObject( getMemberRules() ) {
    
}


/* Clone function */
Plate* Plate::clone(void) const {
    
    return new Plate(*this);
}


/** Get class name of object */
const std::string& Plate::getClassName(void) { 
    
    static std::string rbClassName = "Plate";
    
	return rbClassName; 
}

/** Get class type spec describing type of object */
const TypeSpec& Plate::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( MemberObject::getClassTypeSpec() ) );
    
	return rbClass; 
}

/** Get type spec */
const TypeSpec& Plate::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/* Map calls to member methods */
RbPtr<RbLanguageObject> Plate::executeSimpleMethod(const std::string& name, const std::vector<const RbObject *>& args) {
    
    // special handling for adding a variable
    if (name == "add") {
        
//        // get the argument
//        const RbPtr<Argument>& theArg = args[0];
//        
//        // get the DAG node
//        const RbPtr<const DAGNode> &theNode = theArg->getVariable()->getDagNode();
//        
//        // expand the DAG node
//        // \TODO: We shouldn't use const-casts.
//        const_cast<DAGNode *>( (const DAGNode*) theNode )->setPlate( this );
//        const_cast<DAGNode *>( (const DAGNode*) theNode )->expand();
//        const_cast<DAGNode *>( (const DAGNode*) theNode )->touch();
        
        return NULL;
    }
    else {
        return MemberObject::executeSimpleMethod( name, args );
    }
}

/* Get method specifications */
const MethodTable& Plate::getMethods(void) const {
    
    static MethodTable methods = MethodTable();
    static ArgumentRules* addArgRules = new ArgumentRules();
    
    static bool          methodsSet = false;
    
    if ( methodsSet == false ) {
        
        // add the 'addVariable()' method
        addArgRules->push_back( new ArgumentRule( "var", true, RbObject::getClassTypeSpec() ) );
        
        methods.addFunction("add", new SimpleMemberFunction(RbVoid_name, addArgRules) );
        
        // necessary call for proper inheritance
        methods.setParentTable( &MemberObject::getMethods() );
        methodsSet = true;
    }
    
    return methods;
}


/* Get member rules */
const MemberRules& Plate::getMemberRules(void) const {
    
    static MemberRules memberRules = MemberRules();
    static bool        rulesSet = false;
    
    if (!rulesSet) 
    {
        memberRules.push_back( new ArgumentRule( "size"  , true,  Natural::getClassTypeSpec() ) );
        memberRules.push_back( new ArgumentRule( "parent", false, Plate::getClassTypeSpec(), NULL ) );
        
        rulesSet = true;
    }
    
    return memberRules;
}


size_t Plate::getLength( void ) const {
    return size;
}


const RbPtr<const Plate>& Plate::getParentPlate( void ) const {
    return parent;
}


std::vector<size_t> Plate::getPlateLengths( void ) const {
    
    if ( parent == NULL ) {
        std::vector<size_t> plateLengths;
        plateLengths.push_back( size );
        
        return plateLengths;
    }
    else  {
        std::vector<size_t> plateLengths = parent->getPlateLengths();
        plateLengths.push_back( size );
        
        return plateLengths;
    }
}



/* Print the tree */
void Plate::printValue(std::ostream& o) const {
    
    o << "Plate(i in 1:" << size << ")";
    
}



/** Catch setting of the size variable */
void Plate::setSimpleMemberValue(const std::string& name, const RbPtr<const RbLanguageObject> &var) {
    
    if ( name == "size" ) {
        size = static_cast<const Natural *>( (const RbLanguageObject *)var )->getValue();
    }
    else if ( name == "parent" ) {
        parent = static_cast<const Plate *>( (const RbLanguageObject *)var );
    }
    else {
        MemberObject::setSimpleMemberValue(name, var);
    }
}