//
//  Func_characterMapTree.cpp
//  RevBayes_development_branch
//
//  Created by will freyman on 12/22/16.
//  Copyright (c) 2016 will freyman. All rights reserved.
//


#include "ArgumentRule.h"
#include "OptionRule.h"
#include "ConstantNode.h"
#include "Func_characterMapTree.h"
#include "ModelVector.h"
#include "NexusWriter.h"
#include "Probability.h"
#include "RbException.h"
#include "RevNullObject.h"
#include "RlBranchLengthTree.h"
#include "RlString.h"
#include "RlTimeTree.h"
#include "RlTraceTree.h"
#include "RlAncestralStateTrace.h"
#include "RlUtils.h"
#include "StringUtilities.h"
#include "TreeSummary.h"
#include "TraceTree.h"
#include "AncestralStateTrace.h"
#include "WorkspaceVector.h"

#include <map>
#include <set>
#include <sstream>


using namespace RevLanguage;

/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the process.
 */
Func_characterMapTree* Func_characterMapTree::clone( void ) const
{
    
    return new Func_characterMapTree( *this );
}


/** Execute function */
RevPtr<RevVariable> Func_characterMapTree::execute( void )
{
    
    // get the input tree
    const RevBayesCore::TypedDagNode<RevBayesCore::Tree> *it = static_cast<const Tree&>( this->args[0].getVariable()->getRevObject() ).getDagNode();
    
    // get vector of ancestral state traces
    const WorkspaceVector<AncestralStateTrace>& ast_vector = static_cast<const WorkspaceVector<AncestralStateTrace> &>( args[1].getVariable()->getRevObject() );
    std::vector<RevBayesCore::AncestralStateTrace> ancestralstate_traces;
    for (int i = 0; i < ast_vector.size(); ++i)
    {
        ancestralstate_traces.push_back( ast_vector[i].getValue() );
    }
    
    // get the ancestral state tree trace
    const TraceTree& tt = static_cast<const TraceTree&>( args[2].getVariable()->getRevObject() );
    
    // make a new tree summary object
    RevBayesCore::TreeSummary summary;
    if (args[2].getVariable()->getRevObject() != RevNullObject::getInstance())
    {
        summary = tt.getValue();
    }
    else
    {
        summary = RevBayesCore::TreeSummary();
    }
    
    // get the filename for the tree with MAP character history
    const std::string& map_filename = static_cast<const RlString&>( args[3].getVariable()->getRevObject() ).getValue();
    
    // get the filename for the tree with posteriors for the MAP character history
    const std::string& map_pp_filename = static_cast<const RlString&>( args[4].getVariable()->getRevObject() ).getValue();
    
<<<<<<< HEAD
    int burnin = static_cast<const Integer &>(args[5].getVariable()->getRevObject()).getValue();
<<<<<<< HEAD
    
    int num_time_slices = static_cast<const Integer &>(args[6].getVariable()->getRevObject()).getValue();
    
    bool verbose = static_cast<const RlBoolean &>(args[7].getVariable()->getRevObject()).getValue();
    
    // get the tree with ancestral states
    RevBayesCore::Tree* tree;
    tree = summary.characterMapTree(it->getValue(), ancestralstate_traces, burnin, num_time_slices, verbose);
=======
=======
    int burnin;
    RevObject& b = args[5].getVariable()->getRevObject();
    if ( b.isType( Integer::getClassTypeSpec() ) )
    {
        burnin = static_cast<const Integer &>(b).getValue();
    }
    else
    {
        double burnin_frac = static_cast<const Probability &>(b).getValue();
        burnin = int( floor( ancestralstate_traces[0].size() * burnin_frac ) );
    }
>>>>>>> dc5da38e339e81ba3e116b7fa603df3b16b875cc

    std::string reconstruction = static_cast<const RlString &>(args[6].getVariable()->getRevObject()).getValue();
    bool conditional = false;
    if ( reconstruction == "conditional" )
    {
        conditional = true;
    }
    if ( reconstruction == "joint" )
    {
        throw RbException("Joint ancestral state summaries are not yet implemented. Coming soon!");
    }
    
    int num_time_slices = static_cast<const Integer &>(args[7].getVariable()->getRevObject()).getValue();
    
    bool verbose = static_cast<const RlBoolean &>(args[8].getVariable()->getRevObject()).getValue();
    
    // get the tree with ancestral states
    RevBayesCore::Tree* tree;
    tree = summary.characterMapTree(it->getValue(), ancestralstate_traces, burnin, num_time_slices, conditional, false, verbose);
>>>>>>> development
    
    // write the SIMMAP newick strings
    std::ofstream out_stream;
    if ( map_filename != "" )
    {
        RevBayesCore::Tree t = RevBayesCore::Tree(*tree);
        t.renameNodeParameter("map_character_history", "character_history");
        out_stream.open(map_filename.c_str(), std::fstream::out);
        out_stream << t.getSimmapNewickRepresentation();
        out_stream.close();
    }
    if ( map_pp_filename != "" )
    {
        RevBayesCore::Tree t = RevBayesCore::Tree(*tree);
        t.renameNodeParameter("map_character_history_posteriors", "character_history");
        out_stream.open(map_pp_filename.c_str(), std::fstream::out);
        out_stream << t.getSimmapNewickRepresentation();
        out_stream.close();
    }
    
    // return the tree with annotations
    return new RevVariable( new Tree( tree ) );
}



/** Get argument rules */
const ArgumentRules& Func_characterMapTree::getArgumentRules( void ) const
{
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool rules_set = false;
    
    if (!rules_set)
    {
        
<<<<<<< HEAD
        argumentRules.push_back( new ArgumentRule( "tree",                         Tree::getClassTypeSpec(),                                 "The input tree to summarize ancestral states over.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
=======
        argumentRules.push_back( new ArgumentRule( "tree",                         Tree::getClassTypeSpec(),                                 "The input tree to summarize the character history over.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
>>>>>>> development
        argumentRules.push_back( new ArgumentRule( "ancestral_state_trace_vector", WorkspaceVector<AncestralStateTrace>::getClassTypeSpec(), "A vector of ancestral state traces.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "tree_trace",                   TraceTree::getClassTypeSpec(),                            "A trace of tree samples.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, NULL ) );
        argumentRules.push_back( new ArgumentRule( "character_file",               RlString::getClassTypeSpec(),                             "The name of the file to store the tree annotated with the MAP character history.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "posterior_file",               RlString::getClassTypeSpec(),                             "The name of the file to store the tree annotated with the posterior probabilities for the MAP character history.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
<<<<<<< HEAD
        argumentRules.push_back( new ArgumentRule( "burnin",                       Integer::getClassTypeSpec(),                              "The number of samples to discard as burnin.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Integer(-1) ) );
<<<<<<< HEAD
=======
       std::vector<std::string> reconstruction;
=======
		std::vector<TypeSpec> burnin_types;
        burnin_types.push_back( Probability::getClassTypeSpec() );
        burnin_types.push_back( Integer::getClassTypeSpec() );
        argumentRules.push_back( new ArgumentRule( "burnin"   , burnin_types  , "The fraction/number of samples to discard as burnin.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Probability(0.25) ) );
        std::vector<std::string> reconstruction;
>>>>>>> dc5da38e339e81ba3e116b7fa603df3b16b875cc
        reconstruction.push_back( "conditional" );
        reconstruction.push_back( "joint" );
        reconstruction.push_back( "marginal" );
        argumentRules.push_back( new OptionRule( "reconstruction", new RlString("marginal"), reconstruction, "'joint' and 'conditional' should only be used to summarize character maps sampled from the joint distribution. 'marginal' can be used for character maps sampled from the joint or marginal distribution." ) );
>>>>>>> development
        argumentRules.push_back( new ArgumentRule( "num_time_slices",              Integer::getClassTypeSpec(),                              "The number of time slices to discretize the character history. Should be the same as used for the numeric ODE.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Integer(500) ) );
        argumentRules.push_back( new ArgumentRule( "verbose",                      RlBoolean::getClassTypeSpec(),                            "Printing verbose output", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(true) ) );
        
        rules_set = true;
    }
    
    return argumentRules;
}


/** Get Rev type of object */
const std::string& Func_characterMapTree::getClassType(void)
{
    
    static std::string rev_type = "Func_characterMapTree";
    
    return rev_type;
}


/** Get class type spec describing type of object */
const TypeSpec& Func_characterMapTree::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


/**
 * Get the primary Rev name for this function.
 */
std::string Func_characterMapTree::getFunctionName( void ) const
{
    // create a name variable that is the same for all instance of this class
    std::string f_name = "characterMapTree";
    
    return f_name;
}


/** Get type spec */
const TypeSpec& Func_characterMapTree::getTypeSpec( void ) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    
    return type_spec;
}


/** Get return type */
const TypeSpec& Func_characterMapTree::getReturnType( void ) const
{
    
    static TypeSpec returnTypeSpec = Tree::getClassTypeSpec();
    return returnTypeSpec;
<<<<<<< HEAD
}
=======
}
>>>>>>> development
