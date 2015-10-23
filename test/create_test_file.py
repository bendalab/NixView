import nix
import numpy as np
import matplotlib.pyplot as plt
from IPython import embed


def create_1d_sampled(f, b):
    sample_interval = 0.00005
    t = np.arange(0., 0.5, sample_interval)
    f1 = 100.
    f2 = 200.
    a1 = 0.825
    a2 = 0.4
    eod = a1 * np.sin(t * f1 * 2* np.pi) + a2 * np.sin(f2 * t * 2 * np.pi)
    da = b.create_data_array('eod', 'nix.regular_sampled', data=eod)
    da.unit = 'mV/cm'
    da.label = 'electric field'

    d = da.append_sampled_dimension(sample_interval)
    d.unit = 's'
    d.label = 'time'
    
    sec = f.create_section('in vivo 1','setup')
    hw = sec.create_section('amplifier','hardware.amplifier')
    hw['model'] = 'EXT 2F'
    hw['manufacturer'] = 'npi electronics'
    hw['gain'] = 1000
    
    subj = f.create_section('2015_albi_10', 'subject.animal')
    subj['species'] = 'Apteronotus albifrons'
    subj['sex'] = 'male'
    
    src1 = b.create_source('setup', 'nix.source.setup')
    src1.metadata = sec

    src2 = b.create_source('subject', 'nix.source.subject')
    src2.metadata = subj


def create_1d_range(f, b):
    da = b.data_arrays['eod']
    eod = da[:]
    time = np.asarray(da.dimensions[0].axis(len(eod)))
    shift_eod = np.roll(eod,1) 
    xings = time[(eod < 0) & (shift_eod > 0)]
    
    range_da = b.create_data_array('zero crossings', 'nix.event', data=xings)
    d = range_da.append_alias_range_dimension()
    d.unit = 's'
    d.label = 'time'
    

def create_1d_set(f, b):
    temp = [13.7, 16.3, 14.6, 11.6, 8.6, 5.7, 4., 2.6, 3., 4., 8.5, 13.1]
    labels = ["Sep", "Aug", "Jul", "Jun", "Mai", "April", "Mar", "Feb", "Jan","Dec","Nov","Okt"]
    da =  b.create_data_array("average temperature", "nix.catergorical", data=temp)
    da.label = "temperature"
    da.unit = "C"

    d = da.append_set_dimension()
    d.labels = labels

    src = b.create_source("Data source", "nix.source")
    da.source = src
    
    s = f.create_section("Helgoland Weather data", "data_origin")
    s["period"] = "201509 - 201410"
    s["url"] = "http://www.dwd.de/DE/leistungen/klimadatendeutschland/klimadatendeutschland.html"

    src.metadata = s

def create_m_tag(f,b):
    pass


def create_tag(f, b):
    pass


def create_test_file(filename):
    nix_file = nix.File.open(filename, nix.FileMode.Overwrite)
    
    s = nix_file.create_section('Recording session','recording')
    s['date'] = '2015-10-21'
    s['experimenter'] = 'John Doe'
    
    b = nix_file.create_block("Recording session 1", "nix.recording_session")
    b.metadata  = s
    b2 = nix_file.create_block("Categorical data", "nix.analysis_session") 

    create_1d_sampled(nix_file, b);
    create_1d_range(nix_file, b);
    create_1d_set(nix_file, b2)
    
    nix_file.close()

if __name__ == "__main__":
    create_test_file('nix_test.h5')
