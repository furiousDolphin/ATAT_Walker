


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

app: QApplication = QApplication(sys.argv)
square_wave: Callable[[float], float] = lambda t: 1.0 if np.sin(t) > 0 else 0.0 
u: m.ValueManager = m.ValueManager()
y: m.ValueManager = m.ValueManager()
scope: Oscilloscope = Oscilloscope(y.getter) 
scope.show()

system: m.SecondOrderSystem = m.SecondOrderSystem()  
(zeta, r, f) = system.get_params()
zeta.set_val(0.3)
r.set_val(0.1)
f.set_val(1.0)
u.set_val(square_wave(0.0))
system.set_forcing_func(u.getter)


n: int = 1000
dt:float = 20.0/n
t: float = 0.0

# t_dense: np.ndarray = np.linspace(0, 20, n)
# y_dense = system.step_response(t_dense)

# plot: Plot = Plot((500, 500))
# plot.add(t_dense, y_dense)
# plot.show()

for _ in range(n):
    u.set_val(square_wave(t))
    (t_new, y_new) = system.do_RK4_step(dt)
    y.set_val(y_new)
    t = t_new
    scope.update()
    app.processEvents()
sys.exit(0)
