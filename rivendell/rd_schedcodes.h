/* rd_schedcodes.h
 *
 * Header for the RDSCHEDCODES Structure Rivendell Access Library
 *
 * (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *
 */

#ifndef RD_SCHEDCODES_H
#define RD_SCHEDCODES_H

struct rd_schedcodes {
  char code[11];
  char description[255];
};


#endif  //RD_SCHEDCODES_H
