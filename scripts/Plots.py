
import matplotlib.pyplot as plt
import numpy as np
from typing import NamedTuple, Dict, Any, Tuple
from dataclasses import dataclass, asdict



#----------------------------------------

@dataclass
class Series:

    x: np.ndarray
    y: np.ndarray

    label: str = ""
    color: str = "blue"
    linestyle: str = "-" 
    marker: str = ""
    
#----------------------------------------

@dataclass
class Shape():
    rows: int
    cols: int

    def __iter__(self):
        return iter((self.rows, self.cols))

#----------------------------------------

@dataclass
class PlotMeta():
    xlabel: str = "x"
    ylabel: str = "y"
    title: str = "title"

    def __iter__(self):
        return iter(( self.xlabel, self.ylabel, self.title ))

class IPlot():
    
    def __init__(self, **kwargs):
        self._plot_meta = PlotMeta(**kwargs)
        self._serieses = []

    def add(self, x: np.ndarray, y: np.ndarray, **kwargs)->None:
        self._serieses.append(Series(x, y, **kwargs))

class Plot(IPlot):
    def __init__(self, figsize_px: Tuple[int], dpi: int = 100, **kwargs):
        super().__init__(**kwargs)
        self.__figsize = (figsize_px[0] / dpi, figsize_px[1] / dpi)

    def show(self)->None:

        plt.figure(figsize=self.__figsize)

        for series in self._serieses:

            data: Dict[Any, Any] = asdict(series)
            x: np.ndarray = data.pop("x")
            y: np.ndarray = data.pop("y")

            plt.plot(x, y, **data)

        
        xlabel, ylabel, title = self._plot_meta
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.title(title)
        plt.axis('equal')
        plt.grid(True)
        plt.legend()

        plt.show()

class Subplot(IPlot):
    def show(self, ax)->None:

        if not self._serieses:
            return
        
        for series in self._serieses:

            data: Dict[Any, Any] = asdict(series)
            x: np.ndarray = data.pop("x")
            y: np.ndarray = data.pop("y")

            ax.plot(x, y, **data)

        
        xlabel, ylabel, title = self._plot_meta
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        ax.set_title(title)
        ax.grid(True)
        ax.set_aspect('equal', adjustable='box')
        ax.legend()    

class SubplotManager():
    def __init__(self, figsize_px: Tuple[int], dpi: int = 100, title: str = "", max_cols: int = 3):
        self.__figsize = (figsize_px[0] / dpi, figsize_px[1] / dpi)

        self.__title = title
        self.__max_cols = max_cols

        self.__subplots = []
        self.__shape = Shape(0, 0)
    
    def update_shape(self)->None:

        n = len(self.__subplots)
        if n == 0: return

        cols = int(min(self.__max_cols, np.ceil(np.sqrt(n))))
        rows = int(np.ceil(n / cols))
        self.__shape = Shape(rows, cols)

    def add(self, subplot: Subplot)->None:
        self.__subplots.append(subplot)

    def show(self)->None:

        if not self.__subplots:
            return

        self.update_shape()
        rows, cols = self.__shape
        fig, axes = plt.subplots(rows, cols, figsize=self.__figsize)
        fig.suptitle(self.__title)

        n: int = len(self.__subplots)
        if n == 1:
            axes_flat = [axes]
        else:
            axes_flat = axes.ravel()
        
        for i, subplot in enumerate(self.__subplots):
            ax = axes_flat[i]
            subplot.show(ax)
        fig.tight_layout()
        #fig.show()

        plt.show()

#----------------------------------------



    