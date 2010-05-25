/***
 * hmm_documentation.h
 *
 * A file to include containing documentation for the HMM file formats
 * that can be accessed using the FX system.
 */
#include <fastlib/fastlib.h>

const fx_module_doc hmm_format_doc = {
  NULL, NULL,
  "  MLPACK HMM file formats:\n"
  "\n"
  "The MLPACK HMM classes use three types of files to store the HMM profiles,\n"
  "data sequences, and state sequences.  All of these files are text files and\n"
  "as a result can be easily manipulated and interpreted.  These files consist\n"
  "of an arbitrary number of matrices or vectors where the beginning of a matrix\n"
  "or vector is specified by a line beginning with the '\%' character (the rest\n"
  "of that line can be used for a comment).  Then, each column of the matrix is\n"
  "stored on a single line, with individual values delimited by either spaces or\n"
  "commas.  An example can be seen below:\n"
  "\n"
  "\% a matrix\n"
  "0.006 1.43 4.22\n"
  "4.121 1.44 0.55\n"
  "\n"
  "This would correspond to the matrix\n"
  "  [[0.006 4.121]\n"
  "   [1.43  1.44 ]\n"
  "   [4.22  0.55 ]]\n"
  "\n"
  "The MLPACK HMM classes support three different types of HMM profiles; discrete,\n"
  "Gaussian, and mixture of Gaussians (referred to as 'mixture').\n"
  "\n"
  "  Discrete HMM file format:\n"
  "    The discrete HMM contains two matrices; a transmission probability matrix and\n"
  "    an emission probability matrix.  An example is shown below.\n"
  "\n"
  "    \% Example of a discrete HMM profile\n"
  "    \% transmission matrix (2 states)\n"
  "    0.9 0.05\n"
  "    0.1 0.95\n"
  "    \% emission matrix (2 states x 6 symbols)\n"
  "    0.166 0.1\n"
  "    0.166 0.1\n"
  "    0.166 0.1\n"
  "    0.166 0.1\n"
  "    0.166 0.1\n"
  "    0.17 0.5\n"
  "\n"
  "  Gaussian HMM file format:\n"
  "    The gaussian HMM profile contains a transmission matrix, and then a mean\n"
  "    vector and covariance matrix for each individual state.  An example is shown\n"
  "    below.\n"
  "\n"
  "    \% Example of a gaussian HMM profile\n"
  "    \% transmission matrix (2 states)\n"
  "    0.9 0.05\n"
  "    0.1 0.95\n"
  "    \% mean vector, state 0\n"
  "    0 0\n"
  "    \% covariance matrix, state 0\n"
  "    1.0 0.1\n"
  "    0.0 1.0\n"
  "    \% mean vector, state 1\n"
  "    2 2\n"
  "    \% covariance matrix, state 1\n"
  "    1.0 0.0\n"
  "    0.1 1.0\n"
  "\n"
  "  Mixture HMM file format:\n"
  "    The mixture HMM profile contains a transmission matrix, and then for each\n"
  "    state a prior probability vector for each Gaussian is given; then, for each\n"
  "    Gaussian component, a mean vector and covariance matrix is given.  An example\n"
  "    is given below, for two states with two Gaussian components.\n"
  "\n"
  "    \% Example of a mixture of Gaussians HMM profile\n"
  "    \% transmission matrix (2 states)\n"
  "    0.9 0.05\n"
  "    0.1 0.95\n"
  "    \% prior probability vector, state 0\n"
  "    0.5 0.5\n"
  "    \% mean vector of component 0, state 0\n"
  "    0 0\n"
  "    \% covariance matrix of component 0, state 0\n"
  "    1.0 0.0\n"
  "    0.0 1.0\n"
  "    \% mean vector of component 1, state 0\n"
  "    0 5\n"
  "    \% covariance matrix of component 1, state 0\n"
  "    1.0 0.0\n"
  "    0.0 1.0\n"
  "    \% prior probability vector, state 1\n"
  "    0.3 0.7\n"
  "    \% mean vector of component 0, state 1\n"
  "    5 0\n"
  "    \% covariance matrix of component 0, state 1\n"
  "    1.0 0.0\n"
  "    0.0 1.0\n"
  "    \% mean vector of component 1, state 1\n"
  "    5 5\n"
  "    \% covariance matrix of component 1, state 1\n"
  "    1.0 0.0\n"
  "    0.0 1.0\n"
  "\n"
  "In addition, the MLPACK HMM utilities also use sequence files, which are very\n"
  "similar in format to the HMM file formats.  An example of a discrete sequence\n"
  "can be seen below.  Each sequence is separated by the '\%' character.\n"
  "\n"
  "\% total of 6 symbols\n"
  "\% sequence 1\n"
  "1,2,3,4,5,0,2,3,4,5\n"
  "\% sequence 2\n"
  "3,2,0,2,3,4,5,0,3,4\n"
  "\n"
  "The 'hmm_generate' utility will generate two sequence files indicating the emissions\n"
  "of the HMM in one file, and the hidden variable sequence in the other file.  For\n"
  "instance, the Gaussian model (not the Gaussian mixture model) given as an example\n"
  "above might produce the following emission state sequence:\n"
  "\n"
  "\% state sequence 0 - Vector (10) = \n"
  "0,1,1,1,1,1,1,1,1,1,\n"
  "\n"
  "The corresponding hidden values for each of those ten states might be:\n"
  "\n"
  "\% sequence 0 - Matrix (2 x 10) =\n"
  "0.6080564, 0.6410062,\n"
  "4.287918, 1.159150,\n"
  "4.599946, 7.280909,\n"
  "5.866331, 6.387139,\n"
  "4.641797, 5.300349,\n"
  "5.047448, 6.244166,\n"
  "7.224139, 0.3927096,\n"
  "4.250164, 5.980675,\n"
  "4.772023, 5.894644,\n"
  "5.054193, 4.137040,\n"
};
