


import sys
import os

build_path = os.path.join(os.getcwd(), 'build')
if build_path not in sys.path:
    sys.path.append(build_path)

import numpy as np
from typing import Callable
from PyQt6.QtWidgets import QApplication

import module_uno as m  
from scripts.Plots import Plot
from scripts.Oscilloscope import Oscilloscope

#-------------------------------------------------

oscilloscope_inputs: m.OscilloscopeInputs = m.OscilloscopeInputs()
base_path = os.path.dirname(os.path.abspath(__file__)) + "/"
at_at_app: m.App = m.App(oscilloscope_inputs, base_path)

scope_app: QApplication = QApplication(sys.argv)
scope: Oscilloscope = Oscilloscope(oscilloscope_inputs.u.getter) 
scope.show()

running: bool = True

while running:
    running = at_at_app.run_once()
    scope.update()
    scope_app.processEvents()

sys.exit(0)

#-------------------------------------------------
