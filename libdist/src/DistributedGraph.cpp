/** partitioned graph wrapper -*- C++ -*-
 * @file
 * @section License
 *
 * Galois, a framework to exploit amorphous data-parallelism in irregular
 * programs.
 *
 * Copyright (C) 2017, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT AND WARRANTIES OF
 * PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 *
 * @section Description
 *
 * CPP file for the command line arguments of dGraph. 
 *
 * @author Andrew Lenharth <andrewl@lenharth.org>
 * @author Gurbinder Gill <gurbinder533@gmail.com>
 * @author Roshan Dathathri <roshan@cs.utexas.edu>
 * @author Loc Hoang <l_hoang@utexas.edu>
 */

#include <galois/graphs/DistributedGraph.h>

namespace cll = llvm::cl;

#ifdef __GALOIS_EXP_COMMUNICATION_ALGORITHM__
cll::opt<unsigned> buffSize("sendBuffSize",
                       cll::desc("max size for send buffers in element count"),
                       cll::init(4096),
                       cll::Hidden);
#endif

cll::opt<bool> partitionAgnostic("partitionAgnostic",
                             cll::desc("Do not use partition-aware optimizations"),
                             cll::init(false),
                             cll::Hidden);

// TODO: use enums
cll::opt<DataCommMode> enforce_metadata("metadata",
                             cll::desc("Communication metadata"),
                             cll::values(
                               clEnumValN(noData, "auto",
                                          "Dynamically choose the metadata "
                                          "automatically"),
                               clEnumValN(bitsetData, "bitset",
                                          "Use bitset metadata always"),
                               clEnumValN(offsetsData, "offsets",
                                          "Use offsets metadata always"),
                               clEnumValN(gidsData, "gids",
                                          "Use global IDs metadata always"),
                               clEnumValN(onlyData, "none",
                                          "Do not use any metadata (sends "
                                          "non-updated values)"),
                               clEnumValEnd
                             ),
                             cll::init(noData),
                             cll::Hidden);
DataCommMode enforce_data_mode; // using non-cll type because it can be used 
                                // directly by the GPU

cll::opt<MASTERS_DISTRIBUTION> masters_distribution("balanceMasters",
                             cll::desc("Type of masters distribution."),
                             cll::values(
                               clEnumValN(BALANCED_MASTERS, "nodes",
                                          "Balance nodes only"),
                               clEnumValN(BALANCED_EDGES_OF_MASTERS, "edges",
                                          "Balance edges only (default)"),
                               clEnumValN(BALANCED_MASTERS_AND_EDGES, "both",
                                          "Balance both nodes and edges"),
                               clEnumValEnd
                             ),
                             cll::init(BALANCED_EDGES_OF_MASTERS),
                             cll::Hidden);

cll::opt<uint32_t> nodeWeightOfMaster("nodeWeight",
                             cll::desc("Determines weight of nodes when "
                             "distributing masterst to hosts"),
                             cll::init(0),
                             cll::Hidden);

cll::opt<uint32_t> edgeWeightOfMaster("edgeWeight",
                             cll::desc("Determines weight of edges when "
                             "distributing masters to hosts"),
                             cll::init(0),
                             cll::Hidden);

cll::opt<uint32_t> nodeAlphaRanges("nodeAlphaRanges",
                             cll::desc("Determines weight of nodes when "
                             "partitioning among threads"),
                             cll::init(0),
                             cll::Hidden);

cll::opt<unsigned> numFileThreads("ft",
                             cll::desc("Number of file reading threads or I/O "
                             "requests per host"),
                             cll::init(4),
                             cll::Hidden);

#ifdef __GALOIS_BARE_MPI_COMMUNICATION__
cll::opt<BareMPI> bare_mpi("bare_mpi",
                             cll::desc("Type of bare MPI"),
                             cll::values(
                               clEnumValN(noBareMPI, "no",
                                          "Do not us bare MPI (default)"),
                               clEnumValN(nonBlockingBareMPI, "nonBlocking",
                                          "Use non-blocking bare MPI"),
                               clEnumValN(oneSidedBareMPI, "oneSided",
                                          "Use one-sided bare MPI"),
                               clEnumValEnd
                             ),
                             cll::init(noBareMPI),
                             cll::Hidden);
#endif

cll::opt<unsigned> edgePartitionSendBufSize("edgeBufferSize",
                                 cll::desc("Buffer size for batching edges to "
                                           "send during partitioning."), 
                                 cll::init(32000),
                                 cll::Hidden);
