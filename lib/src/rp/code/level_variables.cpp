/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::level_variables class.
 * \author Sébastien Angibaud
 */
#include "rp/level_variables.hpp"

#include "engine/variable/variable.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief A function that gets a level variable.
 * \param lvl The level.
 * \param n The name of the variable.
 * \param def The default value to return if the variable is not set.
 */
template<typename T>
static T rp_level_variables_get_value
( const bear::engine::level& lvl, const std::string& n, const T& def )
{
  bear::engine::variable<T> var(n);

  if ( lvl.level_variable_exists(var) )
    {
      lvl.get_level_variable(var);
      return var.get_value();
    }
  else
    return def;
} // rp_level_variables_get_value()
