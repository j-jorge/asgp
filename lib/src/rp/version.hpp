/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Version of Andy's Super great Park.
 * \author Sebastien Angibaud
 */
#ifndef __RP_VERSION_HPP__
#define __RP_VERSION_HPP__

#define RP_TO_STR_BIS(v) # v
#define RP_TO_STR(v) RP_TO_STR_BIS(v)

#define RP_MAJOR_VERSION 1
#define RP_MINOR_VERSION 0
#define RP_PATCH_NUMBER 4
#define RP_VERSION_STRING "Andy's Super Great Park " \
  RP_TO_STR(RP_MAJOR_VERSION) "." \
  RP_TO_STR(RP_MINOR_VERSION) "." RP_TO_STR(RP_PATCH_NUMBER)

#endif // __RP_VERSION_HPP__
