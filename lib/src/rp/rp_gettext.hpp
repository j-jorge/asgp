/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Macro to call gettext on the roller painting text domain.
 * \author Julien Jorge
 */
#ifndef __RP_GETTEXT_HPP__
#define __RP_GETTEXT_HPP__

#include <libintl.h>

#define rp_gettext(s) dgettext( "super-great-park", (s) )

#endif // __RP_GETTEXT_HPP__
