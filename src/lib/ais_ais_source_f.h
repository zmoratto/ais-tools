/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_HOWTO_ais_source_f_H
#define INCLUDED_HOWTO_ais_source_f_H

#include <gr_sync_block.h>
#include <gruel/thread.h>

class ais_ais_source_f;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<ais_ais_source_f> ais_ais_source_f_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of ais_ais_source_f.
 *
 * To avoid accidental use of raw pointers, ais_ais_source_f's
 * constructor is private.  ais_make_ais_source_f is the public
 * interface for creating new instances.
 */
ais_ais_source_f_sptr ais_make_ais_source_f (const std::vector<char> &data, int rusk, bool hold);

/*!
 * \brief square2 a stream of floats.
 * \ingroup block
 *
 * This uses the preferred technique: subclassing gr_sync_block.
 */
class ais_ais_source_f : public gr_sync_block
{
private:
  // The friend declaration allows ais_make_ais_source_f to
  // access the private constructor.

  friend ais_ais_source_f_sptr ais_make_ais_source_f (const std::vector<char> &data, int rusk, bool hold);

  ais_ais_source_f (const std::vector<char> &data, int rusk, bool hold);        // private constructor

  std::vector<char>  d_data;
  unsigned int d_offset;

  enum state_t { ST_START, ST_PREAMBLE, ST_STARTSIGN, ST_DATA, ST_CRC, ST_STOPSIGN, ST_SLUTT};

  state_t d_state;
  float d_last;
  char d_plast;
  bool d_hold;
  int d_bitstuffed;
  char d_datalast;
  int d_antallenner;
  unsigned char d_crc[16];
  int ANTALLFOR;
  gruel::mutex d_mutex;

  int d_nstart, d_npreamble, d_nstartsign,  d_ncrc, d_nstopsign, d_nslutt;
  unsigned int d_ndata;
  unsigned short sdlc_crc(unsigned char*, unsigned len);
  void calculate_crc();
public:
  ~ais_ais_source_f (); // public destructor

  // Where all the action really happens
  //  void forecast (int, gr_vector_int &);

  void nypakke(const std::vector<char> &data);

  void restart();

  int work (int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
};

#endif /* INCLUDED_ais_ais_source_f_H */
