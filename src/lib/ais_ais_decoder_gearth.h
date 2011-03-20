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
#ifndef INCLUDED_HOWTO_ais_decoder_gearth_H
#define INCLUDED_HOWTO_ais_decoder_gearth_H

#include <gr_sync_block.h>
#include <gruel/thread.h>
#include <gr_msg_queue.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include "ship.h"
#include <sstream>

class ais_ais_decoder_gearth;

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
typedef boost::shared_ptr<ais_ais_decoder_gearth> ais_ais_decoder_gearth_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of ais_ais_decoder_gearth.
 *
 * To avoid accidental use of raw pointers, ais_ais_decoder_gearth's
 * constructor is private.  ais_make_ais_decoder_gearth is the public
 * interface for creating new instances.
 */
ais_ais_decoder_gearth_sptr ais_make_ais_decoder_gearth (int port);
void *threaden(void*arg);
void *serveren(void *arg);

static char *headerkml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <kml xmlns=\"http://earth.google.com/kml/2.1\"> <Document> <name>aasen.kml</name> <Style id=\"sn_ylw-circle0\"> <IconStyle> <scale>1.1</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/paddle/ylw-circle.png</href> </Icon> <hotSpot x=\"32\" y=\"1\" xunits=\"pixels\" yunits=\"pixels\"/> </IconStyle> </Style> <Style id=\"sh_ylw-circle0\"> <IconStyle> <scale>1.3</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/paddle/ylw-circle.png</href> </Icon> <hotSpot x=\"32\" y=\"1\" xunits=\"pixels\" yunits=\"pixels\"/> </IconStyle> </Style> <StyleMap id=\"msn_ylw-circle\"> <Pair> <key>normal</key> <styleUrl>#sn_ylw-circle0</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_ylw-circle0</styleUrl> </Pair> </StyleMap><Style id=\"sn_red-diamond\"> <IconStyle> <scale>1.1</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/paddle/red-diamond.png</href> </Icon> <hotSpot x=\"32\" y=\"1\" xunits=\"pixels\" yunits=\"pixels\"/> </IconStyle> <ListStyle> <ItemIcon> <href>http://maps.google.com/mapfiles/kml/paddle/red-diamond-lv.png</href> </ItemIcon> </ListStyle> </Style> <Style id=\"sh_red-diamond\"> <IconStyle> <scale>1.3</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/paddle/red-diamond.png</href> </Icon> <hotSpot x=\"32\" y=\"1\" xunits=\"pixels\" yunits=\"pixels\"/> </IconStyle> <ListStyle> <ItemIcon> <href>http://maps.google.com/mapfiles/kml/paddle/red-diamond-lv.png</href> </ItemIcon> </ListStyle> </Style> <StyleMap id=\"msn_red-diamond\"> <Pair> <key>normal</key> <styleUrl>#sn_red-diamond</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_red-diamond</styleUrl> </Pair> </StyleMap>";
static char *bottomkml = "</Document></kml>";

/*!
 * \brief square2 a stream of floats.
 * \ingroup block
 *
 * This uses the preferred technique: subclassing gr_sync_block.
 */
class ais_ais_decoder_gearth : public gr_sync_block
{
private:
  // The friend declaration allows ais_make_ais_decoder_gearth to
  // access the private constructor.

  friend ais_ais_decoder_gearth_sptr ais_make_ais_decoder_gearth (int port);
  friend void* threaden(void*arg);
  friend void* serveren(void*arg);

  ais_ais_decoder_gearth (int port);    // private constructor

  std::vector<char>  d_data;
  unsigned int d_offset;

  enum state_t { ST_SKURR, ST_PREAMBLE, ST_STARTSIGN, ST_DATA, ST_STOPSIGN};
  int d_nskurr, d_npreamble, d_nstartsign, d_ndata,  d_nstopsign;

  state_t d_state;
  unsigned char d_crc[16];
  int d_antallenner;
  unsigned char d_buffer[450];
  unsigned char d_rbuffer[450];
  char *d_tbuffer;
  int d_bufferpos;
  char d_last;
  int d_antallpreamble;
  bool d_bitstuff;
  int d_receivedframes;
  int d_lostframes;
  int d_lostframes2;
  gr_msg_queue_sptr d_msgq;
  FILE *d_outfile;
  std::string d_sendut;
  std::vector<Ship> d_ships;
  pthread_mutex_t d_mutex;
  pthread_mutex_t d_mutex2;
  int d_port;
  void bokstavtabell(unsigned char, unsigned char *, int);


  unsigned short sdlc_crc(unsigned char*, unsigned len);
  bool calculate_crc(int);
  void restart();
  void get_data();
  //    void *threaden(void *arg);
  void update_ship(unsigned long mmsi,double lat,double lon, short heading, short speed);
  void update_shipdata(unsigned long mmsi, std::string name, std::string destination);
  unsigned long henten(int from, int size, unsigned char *frame);
public:
  ~ais_ais_decoder_gearth ();   // public destructor

  // Where all the action really happens
  //  void forecast (int, gr_vector_int &);

  int received();
  int lost();
  int lost2();


  int work (int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
};

#endif /* INCLUDED_ais_ais_decoder_gearth_H */
