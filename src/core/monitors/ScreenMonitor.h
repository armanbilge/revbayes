/**
 * @file
 * This file contains the declaration of ScreenMonitor, used to save information
 * to the screen (std::cout) about the monitoring of a variable DAG node.
 *
 * @brief Declaration of ScreenMonitor
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-11-15 16:03:33 +0100 (Thu, 15 Nov 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-08, version 1.0
 *
 * $Id: ScreenMonitor.h 1833 2012-11-15 15:03:33Z hoehna $
 */

#ifndef ScreenMonitor_H
#define ScreenMonitor_H

#include "Monitor.h"


#include <iostream>
#include <queue>
#include <string>
#include <vector>

namespace RevBayesCore {
        
    class ScreenMonitor : public Monitor {
        
    public:
        // Constructors and Destructors
        ScreenMonitor(DagNode *n, int g, bool pp=true, bool l=true, bool pr=true);                                                                //!< Constructor with single DAG node
        ScreenMonitor(const std::set<DagNode *> &n, int g, bool pp=true, bool l=true, bool pr=true);                                              //!< Constructor with set of DAG node
        ScreenMonitor(const std::vector<DagNode *> &n, int g, bool pp=true, bool l=true, bool pr=true);                                              //!< Constructor with vector of DAG node
        
        // basic methods
        ScreenMonitor*                      clone(void) const;                                                  //!< Clone the object
        
        // Monitor functions
        void                                monitor(unsigned long gen);                                         //!< Monitor at generation gen
        void                                reset(size_t numCycles);
        
        // ScreenMonitor functions
        void                                printHeader(void);                                                  //!< Print header

    private:
        
        // parameters
        bool                                posterior;
        bool                                prior;
        bool                                likelihood;
        bool                                waitingTime;
        std::string                         prefixSeparator;
        std::string                         suffixSeparator;
        size_t                              headerPrintingInterval;
        std::queue<time_t>                  startTimes;
        size_t                              numCycles;                                                          //!< Total number of cycles to monitor
        size_t                              currentGen;
        size_t                              startGen;
        
    };
    
}

#endif

