/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a class to support generation and assignment
           of a random value distributed normally.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
**********************************************************************/
#include "mc_variable_random_normal.hh"

//#include "../../math_utils/include/math_utils.hh" // MathUtils::*

//includes for is_near_eqaul
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include <fenv.h>
#include <limits>
#include <algorithm>
#include <assert.h>

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloVariableRandomNormal::MonteCarloVariableRandomNormal(
    const std::string & var_name,
    unsigned int        seed,
    double              mean,
    double              stdev)
  :
  MonteCarloVariableRandom( var_name, seed),
  max_num_tries(10000),
  distribution(mean, stdev),
  min_value(0.0),
  max_value(0.0),
  truncated_low(false),
  truncated_high(false)
{}

/*****************************************************************************
generate_assignment
Purpose:(generates the normally-distributed random number)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::generate_assignment()
{
  double assignment_d = distribution(random_generator);

  if (truncated_low && truncated_high) {
    if (min_value > max_value) {
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + "  Illegal configuration\nFor variable " + variable_name.c_str() + " the specified minimum allowable value (" + std::to_string(min_value) + ") >= the specified maximum allowable value (" + std::to_string(max_value) + ").\nOne or both of the limits must be changed to generate a random value.\n";
		message_publish(MSG_ERROR, isaacRError.c_str());
                exec_terminate_with_return(1, __FILE__, __LINE__, isaacRError.c_str());
    }
    else if (is_near_equal( min_value, max_value)) {
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + " Overconstrained configuration\nFor variable " + variable_name.c_str() + "  the specified minimum allowable value and \nthe specified maximum allowable value are equal (" + std::to_string(min_value) + ").\nThe distribution collapses to a point.\n";
		message_publish(MSG_WARNING, isaacRError.c_str());
      assignment_d = min_value;
    }
  }

  size_t num_tries = 0;
  while ( (truncated_high && assignment_d > max_value) ||
          (truncated_low  && assignment_d < min_value)) {
    if ( num_tries < max_num_tries) {
      assignment_d = distribution(random_generator);
      num_tries++;
    }
    else {
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + " Random value truncation failure\nCould not generate a value for " + variable_name.c_str() + "  within the specified domain within\nthe specified maximum number of tries (" + std::to_string(max_num_tries) + ").\nAssuming a value equal to:\n - midpoint value for a distribution truncated at both ends\n - truncation value for a distribution truncated at only one end.\n";
		message_publish(MSG_ERROR, isaacRError.c_str());
      // Note - at least one truncation must be defined in order to be in
      // this part of the code.
      if (!truncated_high) { // i.e. truncated-low only
        assignment_d = min_value;
      }
      else if (!truncated_low) { // i.e. truncated-high only
        assignment_d = max_value;
      }
      else { // i.e. truncated both sides
        assignment_d = (max_value + min_value) / 2.0;
      }
      // Stop trying to generate an acceptable value at this point:
      break;
    }
  }
  assign_double(assignment_d);
}

/*****************************************************************************
truncate
Purpose:(Truncates the normal distribution to be within +- limit.)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate(
     double limit,
     TruncationType truncType)
{
  if (limit < 0)
  {
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + " Out-of-domain error\nNegative double-sided truncation specified for variable " + std::to_string(max_num_tries) + "\ntruncate() must receive either two limits or one positive limit!\nUsing absolute value of limit.\nUsing absolute value of limit.\n";
		message_publish(MSG_ERROR, isaacRError.c_str());
    limit = -limit;
  }

  if (is_near_equal(limit, 0.0))
  {
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + " Configuration error\nZero truncation specified for variable " + variable_name.c_str() + "  which will produce a fixed point\n";
		message_publish(MSG_WARNING, isaacRError.c_str());
  }

  // Assign the truncation on both sides:
  truncate_low(-limit, truncType);
  truncate_high(limit, truncType);
}

/***************************************************************************
truncate
Purpose:(Truncates the normal distribution to be within asymmetric limits.)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate(
     double min,
     double max,
     TruncationType truncType)
{
  truncate_low(min, truncType);
  truncate_high(max, truncType);
}

/*****************************************************************************
truncate_low
Purpose:(Specifies the lower-bound of the truncation)
Note - min is often -- but not necessarily -- a negative value.
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate_low(
    double min,
    TruncationType truncType)
{
  switch (truncType) {
   case StandardDeviation:
    min_value = distribution.mean() + distribution.stddev() * min;
    break;
   case Relative:
    min_value = distribution.mean() + min;
    break;
   case Absolute:
    min_value = min;
    break;
   // Unreachable code.  All types are covered.
   default:
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + " Invalid TruncationType\nInvalid truncation type passed to truncate_low for variable " + variable_name.c_str() + ".\nMinimum will not be applied.\n";
		message_publish(MSG_ERROR, isaacRError.c_str());
    return;
  }
  truncated_low = true;
}
/*****************************************************************************
truncate_high
Purpose:(Specifies the upper-bound of the truncation)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::truncate_high( double max,
                                               TruncationType truncType)
{
  switch (truncType) {
   case StandardDeviation:
    max_value = distribution.mean() + distribution.stddev() * max;
    break;
   case Relative:
    max_value = distribution.mean() + max;
    break;
   case Absolute:
    max_value = max;
    break;
   // Unreachable code.  All types are covered.
   default:
		std::string isaacRError = std::string("isaacRError: ") + __FILE__ + " " + std::to_string(__LINE__) + " Invalid TruncationType\nInvalid truncation type passed to truncate_high for variable " + variable_name.c_str() + ".\nMaximum will not be applied.\n";
		message_publish(MSG_ERROR, isaacRError.c_str());
    return;
  }
  truncated_high = true;
}

/*****************************************************************************
untruncate
Purpose:(Remove truncation flags.)
*****************************************************************************/
void
MonteCarloVariableRandomNormal::untruncate()
{
  truncated_low = false;
  truncated_high = false;
}

/*****************************************************************************
 * is_near_equal
 * Purpose: Temporary is_near_equal so math_utls does not have to be used
 * *****************************************************************************/

bool MonteCarloVariableRandomNormal::is_near_equal( float val1, float val2)
{
  float ulp = 0.5f;
  const int fe_prev = fedisableexcept(FE_ALL_EXCEPT);  //temporary disable fp exceptions
  assert(-1 != fe_prev);

  const float abs_val1 = std::abs(val1);
  const float abs_val2 = std::abs(val2);

  bool res = false;

  if (std::min(abs_val1, abs_val2) <= (float)(0.0)) {  //for the zero case
    res = std::max(abs_val1, abs_val2) <= ulp*std::numeric_limits<float>::min()
                                             *std::numeric_limits<float>::epsilon();
  }
  else {
    const float dist = std::abs(val1-val2);
    res = dist <  ulp*std::max(abs_val1, abs_val2)
                     *std::numeric_limits<float>::epsilon() ||  //for the normal number
          dist <= ulp*std::numeric_limits<float>::min()
                     *std::numeric_limits<float>::epsilon();  //for the subnormal number
  }

  feenableexcept(fe_prev); // restore the previous settings of fp exceptions
  return res;
}

bool MonteCarloVariableRandomNormal::is_near_equal( double val1, double val2)
{
  double ulp = 0.5f;
  const int fe_prev = fedisableexcept(FE_ALL_EXCEPT);  //temporary disable fp exceptions
  assert(-1 != fe_prev);

  const double abs_val1 = std::abs(val1);
  const double abs_val2 = std::abs(val2);

  bool res = false;

  if (std::min(abs_val1, abs_val2) <= (double)(0.0)) {  //for the zero case
    res = std::max(abs_val1, abs_val2) <= ulp*std::numeric_limits<double>::min()
                                             *std::numeric_limits<double>::epsilon();
  }
  else {
    const double dist = std::abs(val1-val2);
    res = dist <  ulp*std::max(abs_val1, abs_val2)
                     *std::numeric_limits<double>::epsilon() ||  //for the normal number
          dist <= ulp*std::numeric_limits<double>::min()
                     *std::numeric_limits<double>::epsilon();  //for the subnormal number
  }

  feenableexcept(fe_prev); // restore the previous settings of fp exceptions
  return res;
}
