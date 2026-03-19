

import numpy as np
import pyqtgraph as pg
from PyQt6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget
from PyQt6.QtCore import QTimer
from typing import Callable, Dict, List, Any
from dataclasses import dataclass, asdict

import sys
import os

build_path = os.path.join(os.getcwd(), 'build')
if build_path not in sys.path:
    sys.path.append(build_path)

FloatGetter = Callable[[], float]
FloatSetter = Callable[[float], None]

class Oscilloscope(QMainWindow):
    
    class Series:
        def __init__(self, plot: Any, getter: FloatGetter, name: str = "", pen: str = "", buffer_size: int = 1000):
            self.getter: FloatGetter = getter
            self.val_dense: np.ndarray = np.zeros(buffer_size)
            self.curve: Any = plot.plot(pen=pen, name=name)
        def update(self):
            new_val: float = self.getter()
            self.val_dense = np.roll(self.val_dense, -1)
            self.val_dense[-1] = new_val
            self.curve.setData(self.val_dense)
    
    def __init__(self, serieses_args: List[Dict[str, Any]]):
        super().__init__()

        self.win = pg.GraphicsLayoutWidget(title="title1")
        self.setCentralWidget(self.win)

        self.plot = self.win.addPlot(title="title2")
        self.plot.setYRange(-40.0, 40.0)
        self.plot.addLegend()
        self.plot.showGrid(x=True, y=True)

        self.serieses: List[Any] = []
        for series_args in serieses_args:
            self.serieses.append(self.Series(
                self.plot, 
                series_args["getter"], 
                series_args["name"], 
                series_args["pen"],
                series_args["buffer_size"]))


    def update(self):
        for series in self.serieses:
            series.update()



    
