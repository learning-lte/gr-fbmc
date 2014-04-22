#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 Communications Engineering Lab (CEL), Karlsruhe Institute of Technology (KIT).
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks, fft
import fbmc_swig as fbmc
import pylab as pl

class qa_tx (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
    	L = 8
    	print "test 1 - L =", L
    	num_items = L; # does not refer to symbols but to the number of items the head block lets through
    	overlap = 4 # this is hardcoded and not changeable at the moment
    	symbols = [-0.70710677-0.70710677j, -0.70710677+0.70710677j, -0.70710677-0.70710677j, \
    	           0.70710677+0.70710677j, -0.70710677+0.70710677j,  0.70710677-0.70710677j, \
    	           0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677-0.70710677j, \
    	           0.70710677-0.70710677j,  0.70710677-0.70710677j, -0.70710677-0.70710677j, \
    	           0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677+0.70710677j, \
    	          -0.70710677+0.70710677j, -0.70710677-0.70710677j,  0.70710677+0.70710677j, \
    	          -0.70710677-0.70710677j,  0.70710677-0.70710677j,  0.70710677+0.70710677j, \
    	           0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677+0.70710677j, \
    	           0.70710677+0.70710677j, -0.70710677+0.70710677j, -0.70710677-0.70710677j,\
    	           0.70710677-0.70710677j, -0.70710677+0.70710677j,  0.70710677-0.70710677j,\
    	           0.70710677+0.70710677j, -0.70710677-0.70710677j]
    	self.src = blocks.vector_source_c(symbols, vlen=1, repeat=False)
		#self.head = blocks.head(gr.sizeof_gr_complex,num_items +  L*(4 - ((L/num_items + overlap)%4)))
    	self.s2p = fbmc.serial_to_parallel_cvc(L,L)		
    	self.frame_gen = fbmc.frame_generator_vcvc(sym_len=L,frame_len=len(symbols)/L+4) 
    	self.serialize_iq = fbmc.serialize_iq_vcvc(L)
    	self.apply_betas = fbmc.apply_betas_vcvc(L=L,inverse=0)
    	self.fft = fft.fft_vcc(L, False, (()), False, 1)
		#self.mult_const = blocks.multiply_const_vcc(([L for i in range(L)]))
    	self.ppfb = fbmc.polyphase_filterbank_vcvc(L=L)
    	self.output_commutator = fbmc.output_commutator_vcc(L)
    	self.snk = blocks.vector_sink_c(vlen=1)
    	
    	self.tb.connect(self.src, self.s2p, self.frame_gen)
    	self.tb.connect(self.frame_gen, self.serialize_iq, self.apply_betas)
    	self.tb.connect(self.apply_betas, self.fft, self.ppfb)
    	self.tb.connect(self.ppfb, self.output_commutator, self.snk)
        self.tb.run ()
        
        # check data
        data = self.snk.data()
        ref = ((-0.00044857551619-0.00044857551619j) ,
        (0.000229785631692+9.51803250852e-05j) ,
        (0.000235008650809+0j) ,
        (-0.000786418595356+0.000325745247899j) ,
        (0.0029583569193-0.0029583569193j) ,
        (0.00458181268753-0.0110614743305j) ,
        0j ,
        (0.0101093232545+0.0244060653074j) ,
        (0.0134400202724+0.01254286924j) ,
        (0.00975847802034+0.00423245459431j) ,
        (-0.0215717198951+0j) ,
        (0.00448376796271-0.00120574700489j) ,
        (-0.137716406886+0.132248268564j) ,
        (-0.215114799807+0.499294519548j) ,
        -0.000117504325405j ,
        (-0.488114734649-1.13673473818j) ,
        (-0.676335831811-0.651965572669j) ,
        (-0.25964502846-0.121691428548j) ,
        (-0.999416226968+0.0107757996582j) ,
        (1.81418119521-0.802989724788j) ,
        (-0.151156427158+0.417901879642j) ,
        (0.515256420303-0.261694046575j) ,
        (-0.0107757996582+0.0107858599476j) ,
        (0.456298458913-0.898936890206j) ,
        (-0.121766605211-1.36154673438j) ,
        (0.583015566091+1.07714755376j) ,
        (0.0107657393689-0.47821526633j) ,
        (0.0756685134429+0.743323552321j) ,
        (0.668363949861+0.121318029694j) ,
        (0.651072260902+0.347989657838j) ,
        (0.499766865647+0.499766865647j) ,
        (-1.56545373081-0.117589633267j) ,
        (-0.27973689724+0.84684899308j) ,
        (0.0319435766181+0.682387824446j) ,
        (-0.499766865647-0.988767991924j) ,
        (-0.650533312684-1.5559741121j) ,
        (0.67929418873+0.548652512762j) ,
        (-0.452941798149-0.462707273885j) ,
        (0.0107858599476-0.0107757996582j) ,
        (0.480818026103+0.518103217719j) ,
        (0.0029583569193-0.0143371713047j) ,
        (-1.00977937991-0.55398060774j) ,
        (0.0107757996582-1.02098794686j) ,
        (-1.79142396516+0.770310989561j) ,
        (-0.692285633486-0.689775852083j) ,
        (0.222089662503-0.504623533939j) ,
        (-0.000117504325405+0j) ,
        (-0.0937032563404-0.212396961827j) ,
        (0.141123339322-0.140674763805j) ,
        (0.125196261535+0.0540823171728j) ,
        -0.0213367112443j ,
        (0.0288361498603-0.0125942562723j) ,
        (0.0138885957885+0.0138885957885j) ,
        (-0.00445727499753+0.0107608137503j) ,
        0j ,
        (0.00197669891847+0.0047721733377j) ,
        (-0.0029583569193+0.0029583569193j) ,
        (-0.00189858243861-0.000786418595356j) ,
        0.000235008650809j ,
        (-0.000554751588469+0.000229785631692j) ,
        (-0.00044857551619-0.00044857551619j) ,
        0j ,
        0j ,
        0j )
        max_diff = max(abs(pl.asarray(ref) - data))
        self.assertTrue(max_diff < 0.00001)
    
    def test_002_t (self):
        L = 16
    	print "test 2 - L =", L
    	num_items = L; # does not refer to symbols but to the number of items the head block lets through
    	overlap = 4 # this is hardcoded and not changeable at the moment
    	symbols = [-0.70710677-0.70710677j, -0.70710677+0.70710677j, -0.70710677-0.70710677j, \
    	           0.70710677+0.70710677j, -0.70710677+0.70710677j,  0.70710677-0.70710677j, \
    	           0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677-0.70710677j, \
    	           0.70710677-0.70710677j,  0.70710677-0.70710677j, -0.70710677-0.70710677j, \
    	           0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677+0.70710677j, \
    	          -0.70710677+0.70710677j, -0.70710677-0.70710677j,  0.70710677+0.70710677j, \
    	          -0.70710677-0.70710677j,  0.70710677-0.70710677j,  0.70710677+0.70710677j, \
    	           0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677+0.70710677j, \
    	           0.70710677+0.70710677j, -0.70710677+0.70710677j, -0.70710677-0.70710677j,\
    	           0.70710677-0.70710677j, -0.70710677+0.70710677j,  0.70710677-0.70710677j,\
    	           0.70710677+0.70710677j, -0.70710677-0.70710677j,
    	           -0.70710677-0.70710677j, -0.70710677+0.70710677j, -0.70710677-0.70710677j, \
				   0.70710677+0.70710677j, -0.70710677+0.70710677j,  0.70710677-0.70710677j, \
				   0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677-0.70710677j, \
				   0.70710677-0.70710677j,  0.70710677-0.70710677j, -0.70710677-0.70710677j, \
				   0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677+0.70710677j, \
				  -0.70710677+0.70710677j, -0.70710677-0.70710677j,  0.70710677+0.70710677j, \
				  -0.70710677-0.70710677j,  0.70710677-0.70710677j,  0.70710677+0.70710677j, \
				   0.70710677+0.70710677j,  0.70710677-0.70710677j, -0.70710677+0.70710677j, \
				   0.70710677+0.70710677j, -0.70710677+0.70710677j, -0.70710677-0.70710677j,\
				   0.70710677-0.70710677j, -0.70710677+0.70710677j,  0.70710677-0.70710677j,\
				   0.70710677+0.70710677j, -0.70710677-0.70710677j]
    	self.src = blocks.vector_source_c(symbols, vlen=1, repeat=False)
		#self.head = blocks.head(gr.sizeof_gr_complex,num_items +  L*(4 - ((L/num_items + overlap)%4)))
    	self.s2p = fbmc.serial_to_parallel_cvc(L,L)		
    	self.frame_gen = fbmc.frame_generator_vcvc(sym_len=L,frame_len=len(symbols)/L+4) 
    	self.serialize_iq = fbmc.serialize_iq_vcvc(L)
    	self.apply_betas = fbmc.apply_betas_vcvc(L=L, inverse = 0)
    	self.fft = fft.fft_vcc(L, False, (()), False, 1)
		#self.mult_const = blocks.multiply_const_vcc(([L for i in range(L)]))
    	self.ppfb = fbmc.polyphase_filterbank_vcvc(L=L)
    	self.output_commutator = fbmc.output_commutator_vcc(L)
    	self.snk = blocks.vector_sink_c(vlen=1)
    	
    	self.tb.connect(self.src, self.s2p, self.frame_gen)
    	self.tb.connect(self.frame_gen, self.serialize_iq, self.apply_betas)
    	self.tb.connect(self.apply_betas, self.fft, self.ppfb)
    	self.tb.connect(self.ppfb, self.output_commutator, self.snk)
        self.tb.run ()
        
        # check data
        data = self.snk.data()
        ref = ((-0.00031719079781+0.00031719079781j) ,\
        		(0.000900934021499-0.000401392820075j) ,\
        		(-6.73026550937e-05-0.000162482982711j) ,\
        		(5.77761064396e-06+0.000218540590318j) ,\
        		(0.000166176215045+0j) ,\
        		(-1.92297883138e-05+0.000727374956321j) ,\
        		(0.000230336679856-0.000556081936421j) ,\
        		(-0.00415571469233-0.00185149411608j) ,\
        		(0.00177468349665+0.00209187429446j) ,\
        		(0.000745309743139+0.0018761963596j) ,\
        		(0.00729476978638-0.00344173887295j) ,\
        		(0.0174633904262-0.000205545174898j) ,\
        		8.30881075227e-05j ,\
        		(0.0259521871271-0.000166409617052j) ,\
        		(0.0154545208276+0.00783938125618j) ,\
        		(0.00220055812574-0.0058771094905j) ,\
        		(0.00741165543223-0.0098207205245j) ,\
        		(-0.0266313757512+0.0104609217137j) ,\
        		(0.0132358123431+0.0154830194515j) ,\
        		(0.00574727537474+0.011962938132j) ,\
        		(-0.015336597933-0.00761964121373j) ,\
        		(-0.00790202297629-0.0192932833388j) ,\
        		(-0.0357393411644+0.0512418306346j) ,\
        		(0.198721447097+0.0835395720596j) ,\
        		(-0.0888282504471-0.100423654468j) ,\
        		(-0.0384502193304-0.0873525973076j) ,\
        		(-0.353454017519+0.158761370074j) ,\
        		(-0.805145248047+0.011296509688j) ,\
        		(0.00761964121373-0.00762675491274j) ,\
        		(-1.2122530044+0.0257970085085j) ,\
        		(-0.744528006815-0.382855009608j) ,\
        		(-0.112030539806+0.280389440901j) ,\
        		(-0.387137080859+0.494338009163j) ,\
        		(1.51259682895-0.624183161454j) ,\
        		(-0.682925422504-0.873908626953j) ,\
        		(-0.250562613848+0.179879686649j) ,\
        		(-0.698984167097+0.353388549112j) ,\
        		(0.454068377353-1.46374900476j) ,\
        		(1.09625248745-1.00231525494j) ,\
        		(0.363035051702+0.373041038558j) ,\
        		(-0.557249195678+0.0210989337478j) ,\
        		(-0.110310649334-0.133154724244j) ,\
        		(-0.213933160551-0.826866023661j) ,\
        		(0.362915055524+1.30982330561j) ,\
        		(-0.353388549112-0.353305461005j) ,\
        		(-0.185872099028+0.0827037510689j) ,\
        		(1.2157233981+0.316296461943j) ,\
        		(0.342049137167+0.0760982213452j) ,\
        		(0.310007380894-0.695052587458j) ,\
        		(0.803211488178-0.553173316856j) ,\
        		(-0.844893897653-0.81313095838j) ,\
        		(1.34536082945-0.0781241730198j) ,\
        		(0.353291233607+7.11369901053e-06j) ,\
        		(-0.305779469991-0.598050537105j) ,\
        		(-0.940813116175+1.71691233944j) ,\
        		(-0.199844697791+1.42228157545j) ,\
        		(-1.03789992653-0.961301461696j) ,\
        		(0.08351919578+0.463126474168j) ,\
        		(0.19590838478-0.0671132967698j) ,\
        		(-1.05897736144+0.682131799139j) ,\
        		(-7.11369901053e-06-7.11369901053e-06j) ,\
        		(-1.03970888505+0.460012418712j) ,\
        		(-0.968879928796-0.470035036034j) ,\
        		(-0.142428493701+0.448988827554j) ,\
        		(-0.193834157997+0.308446741734j) ,\
        		(1.57670228549-0.462445956518j) ,\
        		(-0.621722838598-0.752457167279j) ,\
        		(-0.226129446615+0.154276885362j) ,\
        		(-0.706769984525+0.353305461005j) ,\
        		(0.444220446282-1.46018937473j) ,\
        		(1.11182836061-1.02830251295j) ,\
        		(0.373168321537+0.350227294402j) ,\
        		(-0.540334010519+0.0359222446123j) ,\
        		(-0.113519167175-0.146429829592j) ,\
        		(-0.23254482631-0.818410927735j) ,\
        		(0.350753302085+1.29528358655j) ,\
        		(-0.353305461005-0.353388549112j) ,\
        		(-0.215429595994+0.0730704146468j) ,\
        		(1.20520353372+0.31059626264j) ,\
        		(0.340544401928+0.0783669928051j) ,\
        		(0.298411976872-0.681365309142j) ,\
        		(0.828171313243-0.566644847837j) ,\
        		(-0.858916128612-0.830512560321j) ,\
        		(1.33885223149-0.0897038657485j) ,\
        		(0.36862783154+0.00762675491274j) ,\
        		(-0.298106187208-0.578872400261j) ,\
        		(-0.905303560648+1.66622526041j) ,\
        		(-0.398928527052+1.33939468543j) ,\
        		(-0.949706057674-0.861512188823j) ,\
        		(0.12196941511+0.550479071476j) ,\
        		(0.5493624023-0.225874666844j) ,\
        		(-0.253832113389+0.670835289451j) ,\
        		(-0.00762675491274+0.00761964121373j) ,\
        		(0.172544119345+0.434215410203j) ,\
        		(-0.22435192198-0.0871800264264j) ,\
        		(-0.0303979538954+0.168599386653j) ,\
        		(0.192985732064-0.185574076632j) ,\
        		(0.0650063905605+0.161335812116j) ,\
        		(0.0611352812514+0.121288976691j) ,\
        		(0.0244389448441-0.0253842606969j) ,\
        		(-0.00761964121373-8.30881075227e-05j) ,\
        		(-0.00986716085984+0.00428700498385j) ,\
        		(0.0158062098425-0.0265433399435j) ,\
        		(0.00597755514197-0.0246652382727j) ,\
        		(0.0186898686556+0.0169151851589j) ,\
        		(-0.00246320809754-0.0113989089883j) ,\
        		(-0.011316895973+0.00501335705253j) ,\
        		(0.00530163698751-0.0147452642271j) ,\
        		(8.30881075227e-05+0j) ,\
        		(-0.00360530983832-0.00979974603914j) ,\
        		(0.00493465644736+0.00213918195377j) ,\
        		(0.000695822886813-0.00360833803061j) ,\
        		(-0.00418374858892+0.00386655779111j) ,\
        		(-0.00167155068599-0.00301060926783j) ,\
        		(-0.000786418616277-0.00189858248912j) ,\
        		(-0.000761322587426+0.000383245403263j) ,\
        		0j ,\
        		(-0.000228740192706-0.000115146494855j) ,\
        		(-0.000229785637805+0.000554751603227j) ,\
        		(-0.000362382163638+0.00065268203321j) ,\
        		(-0.00063438159562-0.00063438159562j) ,\
        		0j ,\
        		0j ,\
        		0j ,\
        		0j ,\
        		0j ,\
        		0j ,\
        		0j )
        max_diff = max(abs(pl.asarray(ref) - data))
        self.assertTrue(max_diff < 0.00001)
        


if __name__ == '__main__':
    gr_unittest.run(qa_tx)
