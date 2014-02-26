#include "DagNode.h"
#include "FileMonitor.h"
#include "Mcmc.h"
#include "MoveSchedule.h"
#include "PathSampleMonitor.h"
#include "RandomMoveSchedule.h"
#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "RbConstants.h"
#include "RbException.h"
#include "RbMathLogic.h"
#include "RbOptions.h"
#include "SequenctialMoveSchedule.h"
#include "RandomMoveSchedule.h"
#include "ExtendedNewickTreeMonitor.h"
#include "ExtendedNewickAdmixtureTreeMonitor.h"
#include "AdmixtureBipartitionMonitor.h"

#include <cmath>
#include <sstream>
#include <typeinfo>

using namespace RevBayesCore;


/**
 * Constructor. We create an independent copy of the model and thus of all DAG nodes.
 * Someone might have wanted to run another MCMC with different settings on the same model.
 * Thus we also create our own copies of the monitors and moves.
 *
 * \param[in]    m    The model containing all DAG nodes.
 * \param[in]    mvs  The vector of moves.
 * \param[in]    mons The vector of monitors.
 * \param[in]    ca   Is the chain active?
 * \param[in]    ch   The chain heat (temperature).
 * \param[in]    ci   The chain index (for multiple chain, e.g. in MCMCMC).
 */
Mcmc::Mcmc(const Model& m, const std::vector<Move*> &mvs, const std::vector<Monitor*> &mons, bool ca, double ch, int ci) : 
    model( m ), 
    moves(), 
    monitors(), 
    schedule( new RandomMoveSchedule(std::vector<Move*>()) ), 
    chainActive(ca), 
    chainHeat(ch), 
    chainIdx(ci) 
{
    
    // create an independent copy of the model, monitors and moves
    replaceDag(mvs,mons);
    
    initializeChain();
    initializeMonitors();
            
}

/**
 * Copy constructor. For more details see the constructor.
 *
 * \param[in]    m    The MCMC object to copy.
 */
Mcmc::Mcmc(const Mcmc &m) : 
        model( m.model ), 
        moves(), 
        monitors(), 
        schedule( new RandomMoveSchedule(std::vector<Move*>()) ), 
        chainActive(m.chainActive), 
        chainHeat(m.chainHeat), 
        chainIdx(m.chainIdx) 
{
   
    // temporary references
    const std::vector<Monitor*>& mons = m.monitors;
    const std::vector<Move*>& mvs = m.moves;
    
            
    // create an independent copy of the model, monitors and moves
    replaceDag(mvs,mons);
    
    initializeChain();
    initializeMonitors();
}


/**
 * Destructor. Frees the DAG nodes (the model), moves, monitor and the move schedule.
 */
Mcmc::~Mcmc(void) {
    
    // free the moves and monitors
    for (std::vector<Move*>::iterator it = moves.begin(); it != moves.end(); ++it) 
    {
        Move *theMove = (*it);
        delete theMove;
    }
    
    for (std::vector<Monitor*>::iterator it = monitors.begin(); it != monitors.end(); ++it) 
    {
        Monitor *theMonitor = (*it);
        delete theMonitor;
    }
    
    // delete the move schedule
    delete schedule;
}


void Mcmc::burnin(int generations, int tuningInterval) {
    
    // Initialize objects needed by chain
    initializeChain();
    initializeMonitors();
    
    if (chainActive)
    {
        std::cout << "burning in the chain ..." << std::endl;
        std::cout << "0--------25--------50--------75--------100" << std::endl;
        std::cout << "*";
        std::cout.flush();
    }
    
#ifdef DEBUG_MCMC
    std::vector<DagNode *>& dagNodes = model.getDagNodes();
#endif
    
    // reset the counters for the move schedules
    for (std::vector<Move*>::iterator it = moves.begin(); it != moves.end(); ++it) 
    {
        (*it)->resetCounters();
    }
    
    int printInterval = int(fmax(1,generations/20.0));
    
    // Run the chain
    for (int k=1; k<=generations; k++) 
    {
        
        if ( k % printInterval == 0 ) 
        {
            std::cout << "**";
            std::cout.flush();
        }
        
        nextCycle(false);
        
        // check for autotuning
        if ( k % tuningInterval == 0 ) 
        {
            
            // tune the moves
            for (size_t i=0; i<moves.size(); i++) 
            {
                moves[i]->autoTune();
            }
        }
        
    }
    
    if (chainActive)
        std::cout << std::endl;
}


Mcmc* Mcmc::clone( void ) const {
    
    return new Mcmc( *this );
}


double Mcmc::getChainHeat(void)
{
    return chainHeat;
}

size_t Mcmc::getChainIndex(void)
{
    return chainIdx;
}


double Mcmc::getLnPosterior(void)
{
    return lnProbability;
}

double Mcmc::getModelLnProbability(void)
{
    const std::vector<DagNode*> &n = model.getDagNodes();
    double pp = 0.0;
    for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
        //std::cout << (*it)->getName() << "  " << (*it)->getLnProbability() << "\n";
        //(*it)->touch();
        pp += (*it)->getLnProbability();
    }
    return pp;
}


std::vector<Monitor*>& Mcmc::getMonitors(void)
{
    return monitors; 
}



/** Creates a vector of stochastic nodes, starting from the source nodes to the sink nodes */
void Mcmc::getOrderedStochasticNodes(const DagNode* dagNode,  std::vector<DagNode*>& orderedStochasticNodes, std::set<const DagNode*>& visitedNodes) {
    
    if (visitedNodes.find(dagNode) != visitedNodes.end()) { //The node has been visited before
        //we do nothing
        return;
    }
    
    // add myself here for safety reasons
    visitedNodes.insert( dagNode );
    
    if ( dagNode->isConstant() ) { //if the node is constant: no parents to visit
        std::set<DagNode*> children = dagNode->getChildren() ;
        visitedNodes.insert(dagNode);
        std::set<DagNode*>::iterator it;
        for ( it = children.begin() ; it != children.end(); it++ )
            getOrderedStochasticNodes(*it, orderedStochasticNodes, visitedNodes);
    }
    else //if the node is stochastic or deterministic
    { 
        // First I have to visit my parents
        const std::set<const DagNode *>& parents = dagNode->getParents() ;
        std::set<const DagNode *>::const_iterator it;
        for ( it=parents.begin() ; it != parents.end(); it++ ) 
            getOrderedStochasticNodes(*it, orderedStochasticNodes, visitedNodes);
        
        // Then I can add myself to the nodes visited, and to the ordered vector of stochastic nodes
//        visitedNodes.insert(dagNode);
        if ( dagNode->isStochastic() ) //if the node is stochastic
            orderedStochasticNodes.push_back( const_cast<DagNode*>( dagNode ) );
        
        // Finally I will visit my children
        std::set<DagNode*> children = dagNode->getChildren() ;
        std::set<DagNode*>::iterator it2;
        for ( it2 = children.begin() ; it2 != children.end(); it2++ ) 
            getOrderedStochasticNodes(*it2, orderedStochasticNodes, visitedNodes);
    }
    
    return; 
}

void Mcmc::initializeChain( void ) {
    
    std::vector<DagNode *>& dagNodes = model.getDagNodes();
    std::vector<DagNode *> orderedStochNodes;
    std::set< const DagNode *> visited;
    getOrderedStochasticNodes(dagNodes[0],orderedStochNodes, visited );
    
    /* Get initial lnProbability of model */

    // first we touch all nodes so that the likelihood is dirty
    for (std::vector<DagNode *>::iterator i=dagNodes.begin(); i!=dagNodes.end(); i++) 
    {
        (*i)->touch();
    }
    
    
    // redraw parameters for inactive chains in pMC^3 team
    if (chainActive == false)
    {
        for (std::vector<DagNode *>::iterator i=orderedStochNodes.begin(); i!=orderedStochNodes.end(); i++)
        {
            //std::cout << (*i)->getName() << std::endl;
            if ( !(*i)->isClamped() && (*i)->isStochastic() )
            {
                std::cout << "Redrawing values for node " << (*i)->getName() << std::endl;
                (*i)->redraw();
                //                    (*i)->touch(); Not necessary. The distribution will automaticall call touch().
            }
            else if ( (*i)->isClamped() )
            {
                // make sure that the clamped node also recompute their probabilities
                (*i)->touch();
            }
            
        }
    }
    
    int numTries    = 0;
    int maxNumTries = 100;
    for ( ; numTries < maxNumTries; numTries ++ )
    {
        lnProbability = 0.0;
        for (std::vector<DagNode *>::iterator i=dagNodes.begin(); i!=dagNodes.end(); i++) 
        {
            DagNode* node = (*i);
            (*i)->touch();
            
            double lnProb = node->getLnProbability();
            
            if ( !RbMath::isAComputableNumber(lnProb) ) 
            {
                std::cerr << "Could not compute lnProb for node " << node->getName() << "." << std::endl;
                node->printValue(std::cerr,"");
                std::cerr << std::endl;
            }
            lnProbability += lnProb;

        }
        
        // now we keep all nodes so that the likelihood is stored
        for (std::vector<DagNode *>::iterator i=dagNodes.begin(); i!=dagNodes.end(); i++) 
        {
            (*i)->keep();
        }
        
        if ( !RbMath::isAComputableNumber( lnProbability ) )
        {
            std::cerr << "Drawing new initial states ... " << std::endl;
            for (std::vector<DagNode *>::iterator i=orderedStochNodes.begin(); i!=orderedStochNodes.end(); i++) 
            {
                //std::cout << (*i)->getName() << std::endl;
                if ( !(*i)->isClamped() && (*i)->isStochastic() )
                {
                    (*i)->redraw();
//                    (*i)->touch(); Not necessary. The distribution will automaticall call touch().
                }
                else if ( (*i)->isClamped() ) 
                {
                    // make sure that the clamped node also recompute their probabilities
                    (*i)->reInitialized();
                    (*i)->touch();
                }
                
            }
        }
        else
            break;
    }
    
    if ( numTries == maxNumTries )
    {
        std::stringstream msg;
        msg << "Unable to find a starting state with computable probability";
        if ( numTries > 1 )
            msg << " after " << numTries << " tries";
        throw RbException( msg.str() );
        
    }
    
    delete schedule;
    schedule = new RandomMoveSchedule(moves);
    //if (moveSchedule != NULL)
        
    generation = 0;
}



void Mcmc::initializeMonitors(void)
{
    for (size_t i=0; i<monitors.size(); i++)
    {
        monitors[i]->setMcmc(this);
        monitors[i]->setModel( &model );
    }
}



bool Mcmc::isChainActive(void)
{
    return chainActive;
}

void Mcmc::monitor(unsigned long g)
{
    
    // Monitor
    for (size_t i = 0; i < monitors.size(); i++) 
    {
        monitors[i]->monitor( g );
    }
    
}



unsigned long Mcmc::nextCycle(bool advanceCycle) {
    
#ifdef DEBUG_MCMC
    std::vector<DagNode *>& dagNodes = model.getDagNodes();
#endif
    
    size_t proposals = round( schedule->getNumberMovesPerIteration() );
    for (size_t i=0; i<proposals; i++) 
    {
        // Get the move
        Move* theMove = schedule->nextMove( generation );
        
        if ( theMove->isGibbs() ) 
        {
            // do Gibbs proposal
            theMove->performGibbs();
            // theMove->accept(); // Not necessary, because Gibbs samplers are automatically accepted.
        } 
        else 
        {
            // do a Metropolois-Hastings proposal
            
            // Propose a new value
            double lnProbabilityRatio;
            double lnHastingsRatio = theMove->perform(lnProbabilityRatio);
            
            // Calculate acceptance ratio
            double lnR = chainHeat * (lnProbabilityRatio) + lnHastingsRatio;
            
            if (lnR >= 0.0) 
            {
                theMove->accept();
                lnProbability += lnProbabilityRatio;
            }
            else if (lnR < -300.0)
            {
                theMove->reject();
            }
            else 
            {
                double r = exp(lnR);
                // Accept or reject the move
                double u = GLOBAL_RNG->uniform01();
                if (u < r) 
                {
                    theMove->accept();
                    lnProbability += lnProbabilityRatio;
                }
                else 
                {
                    theMove->reject();
                }
            }
        }
        
#ifdef DEBUG_MCMC
        // Assert that the probability calculation shortcuts work
        double curLnProb = 0.0;
        std::vector<double> lnRatio;
        for (std::vector<DagNode*>::iterator i=dagNodes.begin(); i!=dagNodes.end(); i++) 
        {
            (*i)->touch();
            double lnProb = (*i)->getLnProbability();
            curLnProb += lnProb;
        }
        if (fabs(lnProbability - curLnProb) > 1E-8)
            throw RbException("Error in ln probability calculation shortcuts");
        else
            lnProbability = curLnProb;              // otherwise rounding errors accumulate
#endif
    }
    
    
    // advance gen cycle if needed (i.e. run()==true, burnin()==false)
    if (advanceCycle)
        generation++;
    
    // gen number used for p(MC)^3
    return generation;
}


void Mcmc::printOperatorSummary(void) const {
    
    
    // printing the moves summary
    std::cerr << std::endl;
    std::cerr << "                  Name                  | Param              |  Weight  |  Tried   | Accepted | Acc. Ratio| Parameters" << std::endl;
    std::cerr << "===============================================================================================================================" << std::endl;
    for (std::vector<Move*>::const_iterator it = moves.begin(); it != moves.end(); ++it) 
    {
        (*it)->printSummary(std::cerr);
    }
    
    std::cout << std::endl;
}



void Mcmc::replaceDag(const std::vector<Move *> &mvs, const std::vector<Monitor *> &mons)
{

    // we need to replace the DAG nodes of the monitors and moves
    const std::vector<DagNode*>& modelNodes = model.getDagNodes();
    for (std::vector<Move*>::const_iterator it = mvs.begin(); it != mvs.end(); ++it) {
        Move *theMove = (*it)->clone();
        std::set<DagNode*> nodes = theMove->getDagNodes();
        for (std::set<DagNode*>::const_iterator j = nodes.begin(); j != nodes.end(); ++j) {
            
            // error checking
            if ( (*j)->getName() == "" )
                throw RbException( "Unable to connect move to DAG copy because variable name was lost");
            
            DagNode* theNewNode = NULL;
            for (std::vector<DagNode*>::const_iterator k = modelNodes.begin(); k != modelNodes.end(); ++k) {
                if ( (*k)->getName() == (*j)->getName() ) {
                    theNewNode = *k;
                    break;
                }
            }
            // error checking
            if ( theNewNode == NULL ) {
                throw RbException("Cannot find node with name '" + (*j)->getName() + "' in the model but received a move working on it.");
            }
            
            // now swap the node
            theMove->swapNode( *j, theNewNode );
        }
        moves.push_back( theMove );
    }
    
    for (std::vector<Monitor*>::const_iterator it = mons.begin(); it != mons.end(); ++it) 
    {
        Monitor *theMonitor = (*it)->clone();
        std::vector<DagNode*> nodes = theMonitor->getDagNodes();
        for (std::vector<DagNode*>::const_iterator j = nodes.begin(); j != nodes.end(); ++j) 
        {
            
            // error checking
            if ( (*j)->getName() == "" )
                throw RbException( "Unable to connect monitor to DAG copy because variable name was lost");
            
            DagNode* theNewNode = NULL;
            for (std::vector<DagNode*>::const_iterator k = modelNodes.begin(); k != modelNodes.end(); ++k) 
            {
                if ( (*k)->getName() == (*j)->getName() ) 
                {
                    theNewNode = *k;
                    break;
                }
            }
            // error checking
            if ( theNewNode == NULL ) 
            {
                throw RbException("Cannot find node with name '" + (*j)->getName() + "' in the model but received a monitor working on it.");
            }
            
            // now swap the node
            theMonitor->swapNode( *j, theNewNode );
        }
        monitors.push_back( theMonitor );
    }
}


void Mcmc::run(int kIterations) {
    
    // Initialize objects used in run
    initializeChain();
    initializeMonitors();
    
    if ( generation == 0 )
    {
        // Monitor
        startMonitors();
        monitor(0);
    }
    
    // reset the counters for the move schedules
    for (std::vector<Move*>::iterator it = moves.begin(); it != moves.end(); ++it) 
    {
        (*it)->resetCounters();
    }
    
    // Run the chain
    for (int k=1; k<=kIterations; k++) 
    {
        nextCycle(true);
        
        // Monitor
        monitor(generation);
                        
    }
    
    
}




void Mcmc::setChainActive(bool tf)
{
    chainActive = tf;
}

void Mcmc::setChainHeat(double v)
{
    chainHeat = v;
}

void Mcmc::setChainIndex(size_t x)
{
    chainIdx = x;
}


void Mcmc::startMonitors( void ) {
    
    /* Open the output file and print headers */
    for (size_t i=0; i<monitors.size(); i++)
    {
        
        // open filestream for each monitor
        monitors[i]->openStream();
        
        // if this chain is active, print the header
        if (chainActive) // surprised this works properly...
            monitors[i]->printHeader();
    }
}
