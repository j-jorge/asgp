/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#pragma once

#include "rp/transition_effect/level_ending_effect.hpp"

#include "communication/typed_message.hpp"
#include "visual/sprite.hpp"

namespace rp
{
  class level_ending_effect;
  
  class level_capture_progress_message:
    public bear::communication::typed_message< level_ending_effect >
    {
    public:
      explicit level_capture_progress_message( double p );

      bool apply_to( level_ending_effect& that );

    private:
      const double m_progress;
    };
}
