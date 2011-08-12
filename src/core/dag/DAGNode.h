/**
 * @file
 * This file contains the declaration of DAGNode, which is the base
 * class for nodes in a model DAG.
 *
 * @brief Declaration of DAGNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2010-01-12 21:51:54 +0100 (Tis, 12 Jan 2010) $
 * @author The REvBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-16, version 1.0
 * @interface DAGNode
 *
 * $Id: DAGNode.h 221 2010-01-12 20:51:54Z Hoehna $
 */

#ifndef DAGNode_H
#define DAGNode_H

#include "RbInternal.h"
#include "TypeSpec.h"


#include <list>
#include <map>
#include <set>
#include <string>

class ContainerNode;
class Frame;
class RbLanguageObject;
class RbObject;
class VariableNode;
class Variable;
class VectorIndex;
class VectorNatural;
class VectorString;


class DAGNode : public RbInternal {

    friend class                        VariableSlot;                                                           //!< Give VariableSlot exclusive access to slot mgmt functions

    public:
        virtual                        ~DAGNode(void);                                                          //!< Virtual destructor

        // Basic utility functions you should not override
        std::string                     briefInfo(void) const;                                                  //!< Brief info about object
        const std::string&              getValueType(void) const { return valueTypeSpec.getType(); }            //!< Get value type
        const TypeSpec&                 getValueTypeSpec(void) const { return valueTypeSpec; }                  //!< Get value type
        bool                            isValueOfTypeSpec(const TypeSpec& typeSp) const;                        //!< Is DAG node of language type typeSpec?

        // Basic utility functions you may want to override
        virtual bool                    isConst(void) const;                                                    //!< Is DAG node const value?

        // Basic utility functions you have to override
        virtual DAGNode*                clone(void) const = 0;                                                  //!< Clone this node
        virtual const VectorString&     getClass(void) const;                                                   //!< Get DAG node class vector
        virtual const RbLanguageObject* getStoredValue(void) = 0;                                               //!< Get stored value (non-const because of delayed evaluation)
        virtual const RbLanguageObject* getValue(void) = 0;                                                     //!< Get value (non-const because of delayed evaluation)
        virtual RbLanguageObject*       getValuePtr(void) = 0;                                                  //!< Get value pointer (non-const because of delayed evaluation)
        virtual void                    printStruct(std::ostream& o) const = 0;                                 //!< Print struct for user
        virtual void                    printValue(std::ostream& o) = 0;                                        //!< Print value for user (non-const fxn because of delayed evaluation)
        virtual std::string             richInfo(void) const = 0;                                               //!< Complete info about object
    
        // DAG function you have to override
        virtual DAGNode*                cloneDAG(std::map<const DAGNode*, DAGNode*>& newNodes) const = 0;       //!< Clone graph

        // DAG functions you should not have to override
        void                            addChildNode(VariableNode* c);                                          //!< Add child node
        std::set<VariableNode*>&        getChildren(void) { return children; }                                  //!< Return children
        RbObject*                       getElement(size_t index);                                               //!< Get element at index (container function)
        const std::string&              getName(void) const;                                                    //!< get the name
		std::set<DAGNode*>&             getParents(void) { return parents; }                                    //!< Return parents
        Variable*                       getVariable(void) { return variable; }                                  //!< Get the variable owning this node
        bool                            isParentInDAG(const DAGNode* x, std::list<DAGNode*>& done) const;       //!< Is node x a parent of the caller in the DAG?
        size_t                          numberOfChildren(void) const { return children.size(); }                //!< Number of children
        size_t                          numberOfParents(void) const { return parents.size(); }                  //!< Number of parents
        void                            printChildren(std::ostream& o) const;                                   //!< Print children DAG nodes
        void                            printParents(std::ostream& o) const;                                    //!< Print children DAG nodes
        void                            removeChildNode(VariableNode* c);                                       //!< Remove a child node
        void                            setName(const std::string &n) { name = n; }                             //!< Replace the name of the variable
        void                            setVariable(Variable *var);                                             //!< Set the variable owning this node



    protected:
                                        DAGNode(RbLanguageObject* value);                                       //!< Constructor of filled node
                                        DAGNode(const std::string& valType);                                    //!< Constructor of empty node
                                        DAGNode(const DAGNode& x);                                              //!< Copy constructor


        // Member variables keeping track of references
        std::set<VariableNode*>         children;                                                               //!< Set of children nodes
        std::set<DAGNode*>              parents;                                                                //!< Set of parent nodes
        Variable                       *variable;                                                               //!< The variable owning this dag node

        // Member value variables
        const TypeSpec                  valueTypeSpec;                                                          //!< the TypeSpec of the value
        RbLanguageObject*               value;                                                                  //!< Value
        std::string                     name;                                                                   //!< The name/identifier of the DAG node
};

#endif
