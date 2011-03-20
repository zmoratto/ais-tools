#!/usr/bin/env python

from gnuradio import gr, audio, ais, usrp
import sys

if len(sys.argv) != 2:
	print "Usage: ", sys.argv[0], " <frequency> (MHz)"
	sys.exit(-1)

frekvens = float(sys.argv[1])*1000000

fg = gr.flow_graph()

u = usrp.source_c(0)
u.set_decim_rate(256)
subdev_spec = (1,0)
u.set_mux(usrp.determine_rx_mux_value(u,subdev_spec))
subdev = usrp.selected_subdev(u,subdev_spec)
subdev.set_auto_tr(True)
subdev.set_gain(57)
usrp.tune(u,0,subdev,frekvens)#106.3e6)
samplerate = 64000000/256

coeffs = gr.firdes.low_pass(1,samplerate,10000,10000)
filter = gr.fir_filter_ccf(5,coeffs)
gang = gr.multiply_const_ff(10)

lpcoeffs = gr.firdes.low_pass(1,samplerate,8000,3000)
lpfilter = gr.fir_filter_fff(1,lpcoeffs)


demod = gr.quadrature_demod_cf(0.5)

clockrec = gr.clock_recovery_mm_ff(float(samplerate)/5/9600,0.25*0.175*0.175,0.5,0.175,0.005)
datadec = ais.ais_decoder_mysql("localhost","diverse","aisblock","elgelg")

slicer = gr.binary_slicer_fb()

diff = gr.diff_decoder_bb(2)

invert = ais.invert10_bb()


print subdev.name()
fg.connect(u,filter,demod,lpfilter,gang,clockrec,slicer,diff,invert,datadec)

fg.start()

raw_input("Receives AIS from "+ str(frekvens/1000000)+"MHz. " )
fg.stop()
print "Received correct: ",datadec.received()
print "Wrong CRC: ",datadec.lost()
print "Wrong stopsign: ",datadec.lost2()
