#ifndef __PMM_H__
#define __PMM_H__


//====================================================================
/**
  * Set the VCore to a new level
  *
  * \param level       PMM level ID
  */
void SetVCore (unsigned char level);

//====================================================================
/**
  * Set the VCore to a new higher level
  *
  * \param level       PMM level ID
  */
void SetVCoreUp (unsigned char level);

//====================================================================
/**
  * Set the VCore to a new Lower level
  *
  * \param level       PMM level ID
  */
void SetVCoreDown (unsigned char level);

#endif
