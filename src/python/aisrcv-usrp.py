#!/usr/bin/env python

from gnuradio import gr, audio, ais, uhd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys, os, sys

class aisrcv_block(gr.top_block):
    def __init__(self, options, args):
        gr.top_block.__init__(self)

        self.options = options
        self.args = args

        frekvens = options.freq

        # u = usrp.source_c(0)
        # u.set_decim_rate(256)
        # subdev_spec = (1,0)
        # u.set_mux(usrp.determine_rx_mux_value(u,subdev_spec))
        # subdev = usrp.selected_subdev(u,subdev_spec)
        # subdev.set_auto_tr(True)
        # subdev.set_gain(57)
        # usrp.tune(u,0,subdev,frekvens)#106.3e6)
        # samplerate = 64000000/256 # 250000
        self.u = uhd.usrp_source("", uhd.io_type_t.COMPLEX_FLOAT32,1)
        self.u.set_subdev_spec("");   # default should be good?
        self.u.set_samp_rate(250e3);
        samplerate = self.u.get_samp_rate() # Retrieve what it actually does
        self.u.set_antenna("RX2",0)

        if options.gain is None: # set to halfway
            g = self.u.get_gain_range()
            options.gain = (g.start()+g.stop()) / 2.0

        if not(self.u.set_center_freq(frekvens, 0)):
            print "Failed to set frequency"
            sys.exit(1)

        print "Setting gain to %i" % options.gain
        self.u.set_gain(options.gain)

        coeffs = gr.firdes.low_pass(1,samplerate,10000,10000)
        filter = gr.fir_filter_ccf(5,coeffs)
        gang = gr.multiply_const_ff(10)

        lpcoeffs = gr.firdes.low_pass(1,samplerate,8000,3000)
        lpfilter = gr.fir_filter_fff(1,lpcoeffs)
        demod = gr.quadrature_demod_cf(0.5)

        clockrec = gr.clock_recovery_mm_ff(float(samplerate)/5/9600,0.25*0.175*0.175,0.5,0.175,0.005)
        #datadec = ais.ais_decoder_mysql("localhost","diverse","aisblock","elgelg")
        self.datadec = ais.ais_decoder_gearth(30003)

        slicer = gr.binary_slicer_fb()
        diff = gr.diff_decoder_bb(2)
        invert = ais.invert10_bb()
        # print subdev.name()
        self.connect(self.u,filter,demod,lpfilter,gang,clockrec,
                     slicer,diff,invert,self.datadec)


if __name__ == '__main__':
    usage = "%prog: [options]"
    parser = OptionParser(option_class=eng_option, usage=usage)
    parser.add_option("-f", "--freq", type="eng_float", default=161.975e6,
                      help="set the frequency in Hz [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set RF gain" )
    (options, args) = parser.parse_args()

    fg = aisrcv_block(options, args)

    try:
        fg.start()
        raw_input("Receives AIS from "+ str(options.freq/1e6)+"MHz. ")
        fg.stop()
    except KeyboardInterrupt:
        fg.stop()
    print "Received correct: ",fg.datadec.received()
    print "Wrong CRC: ",fg.datadec.lost()
    print "Wrong stopsign: ",fg.datadec.lost2()

