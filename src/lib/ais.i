/* -*- c++ -*- */

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "ais_ais_decoder_gearth.h"
#include "ais_ais_source_f.h"
#include "ais_invert10_bb.h"
#include "ais_ais_decoder_mysql.h"
#include <stdexcept>
%}

// ----------------------------------------------------------------

/*
 * First arg is the package prefix.
 * Second arg is the name of the class minus the prefix.
 *
 * This does some behind-the-scenes magic so we can
 * access ais_square_ff from python as ais.square_ff
 */
// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(ais,ais_source_f);

ais_ais_source_f_sptr ais_make_ais_source_f (const std::vector<char> &data, int rusk, bool hold);

class ais_ais_source_f : public gr_sync_block
{
private:
  ais_source_f (const std::vector<char> &data, int rusk, bool hold);
public:
  void nypakke(const std::vector<char> &data);
  void restart();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(ais,ais_decoder_gearth);

ais_ais_decoder_gearth_sptr ais_make_ais_decoder_gearth (int port);

class ais_ais_decoder_gearth : public gr_sync_block
{
private:
  ais_decoder_gearth (int port);
public:
        int received();
        int lost();
        int lost2();
};



// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(ais,invert10_bb);

ais_invert10_bb_sptr ais_make_invert10_bb ();

class ais_invert10_bb : public gr_sync_block
{
private:
  ais_invert10_bb ();
};

// ----------------------------------------------------------------


GR_SWIG_BLOCK_MAGIC(ais,ais_decoder_mysql);

ais_ais_decoder_mysql_sptr ais_make_ais_decoder_mysql (const char *host,const char *database, const char *user, const char *password);

class ais_ais_decoder_mysql : public gr_sync_block
{
private:
  ais_decoder_mysql (const char *host,const char *database, const char *user, const char *password);
public:
        int received();
        int lost();
        int lost2();
};

