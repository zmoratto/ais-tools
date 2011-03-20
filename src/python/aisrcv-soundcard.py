#!/usr/bin/env python

from gnuradio import gr, audio, ais 
import sys


fg = gr.flow_graph()

samplerate = 48000
src = audio.source(samplerate,"hw:1")
pluss = gr.add_const_ff(0.0)
lpcoeffs = gr.firdes.low_pass(1,samplerate,8000,3000)
lpfilter = gr.fir_filter_fff(1,lpcoeffs)

forsterk = gr.multiply_const_ff(5)
clockrec = gr.clock_recovery_mm_ff(float(samplerate)/9600,0.25*0.175*0.175,0.5,0.175,0.005)

slicer = gr.binary_slicer_fb()
# Parameters: ([mysql-server],[database name],[database user],[database password])
datadec = ais.ais_decoder_mysql("localhost","diverse","ruben","elg")
# You should use the create_mysql.sql to create the necessary tables 
# in the database.
# See create_mysql.txt for help. Those files can be found in the root folder of
# the source tree.
diff = gr.diff_decoder_bb(2)

invert = ais.invert10_bb()

fg.connect(src,pluss,lpfilter,forsterk,clockrec,slicer,diff,invert,datadec)

fg.start()

raw_input("Receives AIS from soundcard ")
fg.stop()
print "Received correctly: ",datadec.received()
print "Wrong CRC: ",datadec.lost()
print "Wrong Size: ",datadec.lost2()

