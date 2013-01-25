/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief A message that adds a speaker in a balloon layer.
 * \author Angibaud Sébastien
 */
#ifndef __RP_HELP_LAYER_STARTING_MESSAGE_HPP__
#define __RP_HELP_LAYER_STARTING_MESSAGE_HPP__

#include "communication/typed_message.hpp"

namespace rp
{
  class help_layer;
  
  /**
   * \brief A message that starts the help layer.
   * \author Sébastien Angibaud
   */
  class help_layer_starting_message :
    public bear::communication::typed_message<help_layer>
    {
    public:
      help_layer_starting_message();
      bool apply_to( help_layer& that );
    }; // class help_layer_starting_message
} // namespace rp

#endif // __RP_HELP_LAYER_STARTING_MESSAGE_HPP__
