/**
 * @file smorms3.hpp
 * @author Vivek Pal
 *
 * SMORMS3 i.e. squared mean over root mean squared cubed optimizer. It is a
 * hybrid of RMSprop, which estimates a safe and optimal distance based on
 * curvature and Yann LeCun’s method in "No more pesky learning rates".
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_OPTIMIZERS_SMORMS3_SMORMS3_HPP
#define MLPACK_CORE_OPTIMIZERS_SMORMS3_SMORMS3_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/core/optimizers/sgd/sgd.hpp>

#include "smorms3_update.hpp"

namespace mlpack {
namespace optimization {

/**
 * SMORMS3 is an optimizer that estimates a safe and optimal distance based on
 * curvature and normalizing the stepsize in the parameter space. It is a hybrid
 * of RMSprop and Yann LeCun’s method in "No more pesky learning rates".
 *
 * For more information, see the following.
 *
 * @code
 * @misc{Funk2015,
 *   author    = {Simon Funk},
 *   title     = {RMSprop loses to SMORMS3 - Beware the Epsilon!},
 *   year      = {2015}
 * }
 * @endcode
 *
 *
 * For SMORMS3 to work, a DecomposableFunctionType template parameter is required.
 * This class must implement the following function:
 *
 *   size_t NumFunctions();
 *   double Evaluate(const arma::mat& coordinates, const size_t i);
 *   void Gradient(const arma::mat& coordinates,
 *                 const size_t i,
 *                 arma::mat& gradient);
 *
 * NumFunctions() should return the number of functions (\f$n\f$), and in the
 * other two functions, the parameter i refers to which individual function (or
 * gradient) is being evaluated.  So, for the case of a data-dependent function,
 * such as NCA (see mlpack::nca::NCA), NumFunctions() should return the number
 * of points in the dataset, and Evaluate(coordinates, 0) will evaluate the
 * objective function on the first point in the dataset (presumably, the dataset
 * is held internally in the DecomposableFunctionType).
 *
 * @tparam DecomposableFunctionType Decomposable objective function type to be
 *     minimized.
 */
template<typename DecomposableFunctionType>
class SMORMS3
{
 public:
  /**
   * Construct the SMORMS3 optimizer with the given function and parameters. The
   * defaults here are not necessarily good for the given problem, so it is
   * suggested that the values used be tailored to the task at hand.  The
   * maximum number of iterations refers to the maximum number of points that
   * are processed (i.e., one iteration equals one point; one iteration does not
   * equal one pass over the dataset).
   *
   * @param function Function to be optimized (minimized).
   * @param stepSize Step size for each iteration.
   * @param epsilon Value used to initialise the mean squared gradient parameter.
   * @param maxIterations Maximum number of iterations allowed (0 means no
   *        limit).
   * @param tolerance Maximum absolute tolerance to terminate algorithm.
   * @param shuffle If true, the function order is shuffled; otherwise, each
   *        function is visited in linear order.
   */
  SMORMS3(DecomposableFunctionType& function,
      const double stepSize = 0.001,
      const double epsilon = 1e-16,
      const size_t maxIterations = 100000,
      const double tolerance = 1e-5,
      const bool shuffle = true);

  /**
   * Optimize the given function using SMORMS3. The given starting point will 
   * be modified to store the finishing point of the algorithm, and the final
   * objective value is returned.
   *
   * @param iterate Starting point (will be modified).
   * @return Objective value of the final point.
   */
  double Optimize(arma::mat& iterate) { return optimizer.Optimize(iterate); }

  //! Get the instantiated function to be optimized.
  const DecomposableFunctionType& Function() const { return function; }
  //! Modify the instantiated function.
  DecomposableFunctionType& Function() { return function; }

  //! Get the step size.
  double StepSize() const { return optimizer.StepSize(); }
  //! Modify the step size.
  double& StepSize() { return optimizer.StepSize(); }

  //! Get the value used to initialise the mean squared gradient parameter.
  double Epsilon() const { return optimizer.Epsilon(); }
  //! Modify the value used to initialise the mean squared gradient parameter.
  double& Epsilon() { return optimizer.Epsilon(); }

  //! Get the maximum number of iterations (0 indicates no limit).
  size_t MaxIterations() const { return optimizer.MaxIterations(); }
  //! Modify the maximum number of iterations (0 indicates no limit).
  size_t& MaxIterations() { return optimizer.MaxIterations(); }

  //! Get the tolerance for termination.
  double Tolerance() const { return optimizer.Tolerance(); }
  //! Modify the tolerance for termination.
  double& Tolerance() { return optimizer.Tolerance(); }

  //! Get whether or not the individual functions are shuffled.
  bool Shuffle() const { return optimizer.Shuffle(); }
  //! Modify whether or not the individual functions are shuffled.
  bool& Shuffle() { return optimizer.Shuffle(); }

 private:
  //! The instantiated function.
  DecomposableFunctionType& function;

  //! The step size for each example.
  double stepSize;

  //! The value used to initialise the mean squared gradient parameter.
  double epsilon;

  //! The maximum number of allowed iterations.
  size_t maxIterations;

  //! The tolerance for termination.
  double tolerance;

  //! Controls whether or not the individual functions are shuffled when
  //! iterating.
  bool shuffle;

  //! The Stochastic Gradient Descent object with SMORMS3 update policy.
  SGD<DecomposableFunctionType, SMORMS3Update> optimizer;
};

} // namespace optimization
} // namespace mlpack

// Include implementation.
#include "smorms3_impl.hpp"

#endif
