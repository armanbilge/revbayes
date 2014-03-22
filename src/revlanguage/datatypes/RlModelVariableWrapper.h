/**
 * @file
 * This file contains the declaration of the RevLanguage model variable wrapper, which is
 * the RevLanguage abstract base class for all language objects wrapping core datatypes that can be stored inside
 * DAG nodes and hence used in model graphs.
 *
 * @brief Declaration of RlModelVariableWrapper
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date: 2012-08-03 17:51:49 +0200 (Fri, 03 Aug 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @since Version 1.0, 2012-08-06
 *
 * $Id: RbLanguageObject.h 1734 2012-08-03 15:51:49Z hoehna $
 */

#ifndef RlModelVariableWrapper_H
#define RlModelVariableWrapper_H

#include "MethodTable.h"
#include "RbLanguageObject.h"
#include "TypedDagNode.h"

namespace RevLanguage {
    
    template <typename rbType>
    class RlModelVariableWrapper : public RbLanguageObject {
    
    public:
        virtual                                ~RlModelVariableWrapper(void);                                              //!< Virtual destructor

        RlModelVariableWrapper&                 operator=(const RlModelVariableWrapper& x);                                 //!< Assignment operator    
        
        // the value type definition
        typedef rbType valueType;
       
        // Basic utility functions you have to override
        virtual RlModelVariableWrapper*         clone(void) const = 0;                                                      //!< Clone object
        RlModelVariableWrapper<rbType>*         dagReference(void);                                                         //!< Make reference to object
    
        // function you might want to overwrite
        virtual RbLanguageObject*               executeMethod(const std::string& name, const std::vector<Argument>& args);  //!< Override to map member methods to internal functions
        virtual RbLanguageObject*               getMember(const std::string& name) const;                                   //!< Get member variable 
        virtual const MethodTable&              getMethods(void) const;                                                     //!< Get member methods (const)
        virtual bool                            hasMember(const std::string& name) const;                                   //!< Has this object a member with name

        // Basic utility functions you should not have to override
        RevBayesCore::TypedDagNode<rbType>*     getValueNode(void) const;
        bool                                    isConstant(void) const;                                                     //!< Is this variable and the internally stored deterministic node constant?
        void                                    makeConstantValue();                                                        //!< Convert the stored variable to a constant variable (if applicable)
        virtual void                            printStructure(std::ostream& o) const;                                      //!< Print structure of language object for user
        void                                    printValue(std::ostream& o) const;                                          //!< Print value for user
        void                                    setName(const std::string &n);                                              //!< Set the name of the variable (if applicable)
        void                                    replaceVariable(RbLanguageObject *newVar);                                  //!< Replace the internal DAG node

        RlModelVariableWrapper<rbType>*         shallowCopy(void);      // Testing...
        
        // getters and setters
        virtual const rbType&                   getValue(void) const;
        void                                    setValue(rbType *x);
        
    protected:
        RlModelVariableWrapper(void);
        RlModelVariableWrapper(rbType *v);
        RlModelVariableWrapper(RevBayesCore::TypedDagNode<rbType> *v);
        RlModelVariableWrapper(const RlModelVariableWrapper &v);
        
        RevBayesCore::TypedDagNode<rbType>*     value;
        mutable MethodTable                     methods;
    
    private:
        
        void                                    initMethods(void);
    };
    
}


#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "Cloner.h"
#include "ConstantNode.h"
#include "DeterministicNode.h"
#include "MemberFunction.h"
#include "RlUtils.h"
#include "StochasticNode.h"
#include "TypedReferenceFunction.h"

template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>::RlModelVariableWrapper() : RbLanguageObject(), 
    value( NULL ), 
    methods() 
{
    
//    initMethods();
    
}



template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>::RlModelVariableWrapper(rbType *v) : RbLanguageObject(), 
    value( new RevBayesCore::ConstantNode<rbType>("",v) ), 
    methods() 
{
    // increment the reference count to the value
    value->incrementReferenceCount();
    
//    initMethods();
    
}



template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>::RlModelVariableWrapper(RevBayesCore::TypedDagNode<rbType> *v) : RbLanguageObject(), 
    value( v ), 
    methods() 
{
    
    // increment the reference count to the value
    value->incrementReferenceCount();
    
    
//    initMethods();
    
}



template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>::RlModelVariableWrapper(const RlModelVariableWrapper &v) : RbLanguageObject(), 
    value( NULL ), 
    methods() 
{
    
    if ( v.value != NULL ) 
    {
        
        value = v.value->clone();
        
        // increment the reference count to the value
        value->incrementReferenceCount();
    }
    
//    initMethods();
    
}



template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>::~RlModelVariableWrapper() 
{
    
    // free the old value
    if ( value != NULL ) 
    {
        if ( value->decrementReferenceCount() == 0 ) 
        {
            delete value;
        }
    }
}


template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>& RevLanguage::RlModelVariableWrapper<rbType>::operator=(const RlModelVariableWrapper &v) {
    
    if ( this != &v ) 
    {
        // free the memory
        // free the old value
        if ( value != NULL ) 
        {
            if ( value->decrementReferenceCount() == 0 ) 
            {
                delete value;
            }
            
            value = NULL;
        }
        
        // create own copy
        if ( v.value != NULL ) 
        {
            value = v.value->clone();
            
            // increment the reference count to the value
            value->incrementReferenceCount();
        }
    }
    
    return *this;
}


template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>* RevLanguage::RlModelVariableWrapper<rbType>::dagReference(void) {
    
    RevBayesCore::TypedReferenceFunction< rbType >* f = new RevBayesCore::TypedReferenceFunction< rbType >(value);
    RevBayesCore::DeterministicNode< rbType >* newVal = new RevBayesCore::DeterministicNode< rbType >( "", f );
    
    RevLanguage::RlModelVariableWrapper<rbType>* newObj = this->clone();

    // replace value with deterministic reference and attempt memory management
    newObj->value->decrementReferenceCount();
    newObj->value = newVal;
    newObj->value->incrementReferenceCount();

    return newObj;
}


/* Map calls to member methods */
template <typename rbType>
RevLanguage::RbLanguageObject* RevLanguage::RlModelVariableWrapper<rbType>::executeMethod(std::string const &name, const std::vector<Argument> &args) {
    
    if (name == "clamp") 
    {
        
        // check whether the variable is actually a stochastic node
        if ( !value->isStochastic() ) 
        {
            throw RbException("Only stochastic variables can be clamped.");
        }
        // convert the node
        RevBayesCore::StochasticNode<rbType>* stochNode = static_cast<RevBayesCore::StochasticNode<rbType> *>( value );
        
        // get the observation
        const rbType &observation = static_cast<const RlModelVariableWrapper<rbType> &>( args[0].getVariable()->getValue() ).getValue();
        
        // clamp
        stochNode->clamp( RevBayesCore::Cloner<rbType, IsDerivedFrom<rbType, RevBayesCore::Cloneable>::Is >::createClone( observation ) );
        
        return NULL;
    } 
    else if (name == "setValue") 
    {
        
        // check whether the variable is actually a stochastic node
        if ( !value->isStochastic() ) 
        {
            throw RbException("You can only set the value for stochastic variables.");
        }
        // convert the node
        RevBayesCore::StochasticNode<rbType>* stochNode = static_cast<RevBayesCore::StochasticNode<rbType> *>( value );
        
        // get the observation
        const rbType &observation = static_cast<const RlModelVariableWrapper<rbType> &>( args[0].getVariable()->getValue() ).getValue();
        
        // set value
        stochNode->setValue( RevBayesCore::Cloner<rbType, IsDerivedFrom<rbType, RevBayesCore::Cloneable>::Is >::createClone( observation ) );
        
        return NULL;
    } 
    else if (name == "redraw") 
    {
        
        // check whether the variable is actually a stochastic node
        if ( !value->isStochastic() ) 
        {
            throw RbException("You can only set the value for stochastic variables.");
        }
        // convert the node
        RevBayesCore::StochasticNode<rbType>* stochNode = static_cast<RevBayesCore::StochasticNode<rbType> *>( value );
                
        // redraw the value
        stochNode->redraw();
        
        return NULL;
    } 
    
    return RbLanguageObject::executeMethod( name, args );
}


/* Find member variables */
template <typename rbType>
RevLanguage::RbLanguageObject* RevLanguage::RlModelVariableWrapper<rbType>::getMember(std::string const &name) const
{
    
    // check whether the variable is actually a stochastic node
    if ( value->isStochastic() ) 
    {
        if ( name == "prob" || name == "probability" ) 
        {
            // convert the node
            RevBayesCore::StochasticNode<rbType>* stochNode = static_cast<RevBayesCore::StochasticNode<rbType> *>( value );
            double lnProb = stochNode->getLnProbability();
            RbLanguageObject *p = RlUtils::RlTypeConverter::toReal( exp(lnProb) );
            
            return p;
        } 
        else if ( name == "lnProb" || name == "lnProbability" ) 
        {
            // convert the node
            RevBayesCore::StochasticNode<rbType>* stochNode = static_cast<RevBayesCore::StochasticNode<rbType> *>( value );
            double lnProb = stochNode->getLnProbability();
            RbLanguageObject *p = RlUtils::RlTypeConverter::toReal( lnProb );
            
            return p;
            
        }
    }

    
    return RbLanguageObject::getMember( name );
}


/* Get method specifications */
template <typename rbType>
const RevLanguage::MethodTable&  RevLanguage::RlModelVariableWrapper<rbType>::getMethods(void) const {
    
//    static MethodTable methods      = MethodTable();
    // Sebastian: Static variables don't work because derived classes, e.g. PosReal from Real
    // require different types but only one static variable will be set for both classes!!!
//    static bool        methodsSet   = false;
    
//    if ( methodsSet == false ) 
//    {
    
    methods = MethodTable();
        ArgumentRules* clampArgRules = new ArgumentRules();
        clampArgRules->push_back( new ArgumentRule("x", true, getTypeSpec() ) );
        methods.addFunction("clamp", new MemberFunction( RlUtils::Void, clampArgRules) );
        
        ArgumentRules* setValueArgRules = new ArgumentRules();
        setValueArgRules->push_back( new ArgumentRule("x", true, getTypeSpec() ) );
        methods.addFunction("setValue", new MemberFunction( RlUtils::Void, setValueArgRules) );
        
        ArgumentRules* redrawArgRules = new ArgumentRules();
        methods.addFunction("redraw", new MemberFunction( RlUtils::Void, redrawArgRules) );
        
        // necessary call for proper inheritance
        methods.setParentTable( &RbLanguageObject::getMethods() );
//        methodsSet = true;
//    }
    
    return methods;
}



template <typename rbType>
const rbType& RevLanguage::RlModelVariableWrapper<rbType>::getValue( void ) const {
    
    return value->getValue();
}



template <typename rbType>
RevBayesCore::TypedDagNode<rbType>* RevLanguage::RlModelVariableWrapper<rbType>::getValueNode( void ) const {
    
    return value;
}



/**
 * Has this object a member with the given name?
 *
 */
template<typename rbType>
bool RevLanguage::RlModelVariableWrapper<rbType>::hasMember(std::string const &name) const 
{
    // first the general members ...
    // if ( name == )
    
    // members that all stochastic variables have
    if ( value->isStochastic() )  
    {
        if ( name == "prob" || name == "probability" ) 
        {
            return true;
        } 
        else if ( name == "lnProb" || name == "lnProbability" ) 
        {
            return true;
        }
    } 
//    else 
//    {
//        <#statements-if-false#>
//    }
    
    return false;
}


template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::initMethods( void )
{
    
    ArgumentRules* clampArgRules = new ArgumentRules();
    clampArgRules->push_back( new ArgumentRule("x", true, getTypeSpec() ) );
    methods.addFunction("clamp", new MemberFunction( RlUtils::Void, clampArgRules) );
    
    ArgumentRules* setValueArgRules = new ArgumentRules();
    setValueArgRules->push_back( new ArgumentRule("x", true, getTypeSpec() ) );
    methods.addFunction("setValue", new MemberFunction( RlUtils::Void, setValueArgRules) );
    
    ArgumentRules* redrawArgRules = new ArgumentRules();
    methods.addFunction("redraw", new MemberFunction( RlUtils::Void, redrawArgRules) );
    
    // necessary call for proper inheritance
    methods.setParentTable( &RbLanguageObject::getMethods() );

}


template <typename rbType>
bool RevLanguage::RlModelVariableWrapper<rbType>::isConstant( void ) const {
    
    return value->isConstant();
}


template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::makeConstantValue( void ) {
    
    if ( value == NULL )
    {
        throw RbException("Cannot convert a variable without value to a constant value.");
    }
    else
    {
        // @todo: we might check if this variable is already constant. Now we construct a new value anyways.
        RevBayesCore::ConstantNode<rbType>* newVal = new RevBayesCore::ConstantNode<rbType>(value->getName(), RevBayesCore::Cloner<rbType, IsDerivedFrom<rbType, RevBayesCore::Cloneable>::Is >::createClone( value->getValue() ) );
        value->replace(newVal);
        
        // delete the value if there are no other references to it.
        if ( value->decrementReferenceCount() == 0 )
        {
            delete value;
        }
        
        value = newVal;
        
        // increment the reference counter
        value->incrementReferenceCount();
    }
    
}


/** Print structure info for user */
template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::printStructure(std::ostream &o) const {
    
    value->printStructureInfo(o);
}


/** Print value for user */
template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::printValue(std::ostream &o) const {
    
    value->printValue(o,"");
}


template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::replaceVariable(RbLanguageObject *newVar) {
    
    RevBayesCore::DagNode* newParent = newVar->getValueNode();
    
    while ( value->getNumberOfChildren() > 0 ) 
    {
        value->getFirstChild()->swapParent(value, newParent);
    }
    
}


template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::setName(std::string const &n) {
    
    if ( value == NULL ) 
    {
        throw RbException("Null-pointer-exception: Cannot set name of value.");
    } 
    else 
    {
        value->setName( n );
    }
    
}



template <typename rbType>
void RevLanguage::RlModelVariableWrapper<rbType>::setValue(rbType *x) {
    
    RevBayesCore::ConstantNode<rbType>* newVal;
    
    if ( value == NULL ) 
    {
        newVal = new RevBayesCore::ConstantNode<rbType>("",x);
    } 
    else 
    {
        newVal = new RevBayesCore::ConstantNode<rbType>(value->getName(),x);
        value->replace(newVal);

        if ( value->decrementReferenceCount() == 0 ) 
        {
            delete value;
        }
        
    }
    
    value = newVal;
    
    // increment the reference count to the value
    value->incrementReferenceCount();
    
}


template <typename rbType>
RevLanguage::RlModelVariableWrapper<rbType>* RevLanguage::RlModelVariableWrapper<rbType>::shallowCopy(void) {
    
}

#endif
