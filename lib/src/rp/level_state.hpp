#ifndef RP_LEVEL_STATE_HPP
#define RP_LEVEL_STATE_HPP

namespace rp
{
  struct level_state
  {
    static constexpr unsigned int locked = 0;
    static constexpr unsigned int unlocked = 1;
    static constexpr unsigned int completed = 2;
    static constexpr unsigned int bronze = 3;
    static constexpr unsigned int silver = 4;
    static constexpr unsigned int gold = 5;
  };
}

#endif
