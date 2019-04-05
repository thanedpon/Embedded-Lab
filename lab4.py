from PyQt5 import QtCore, QtGui, QtWidgets
import sys
import serial
import numpy as np
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.animation import TimedAnimation

import random


class Ui_MainWindow(QtWidgets.QMainWindow):

    def __init__(self,parent=None):
        super(Ui_MainWindow,self).__init__(parent=parent)
        self.setupUi(self)

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1126, 600)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.horizontalLayoutWidget = QtWidgets.QWidget(self.centralwidget)
        self.horizontalLayoutWidget.setGeometry(QtCore.QRect(0, 0, 1121, 591))
        self.horizontalLayoutWidget.setObjectName("horizontalLayoutWidget")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout()
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.pushButton = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton.setObjectName("pushButton")
        self.verticalLayout_2.addWidget(self.pushButton)
        self.widget_3 = PlotCanvas(self.horizontalLayoutWidget)
        self.widget_3.setStyleSheet("background-color: rgb(255, 255, 255);")
        self.widget_3.setObjectName("widget_3")
        self.verticalLayout_2.addWidget(self.widget_3)
        self.horizontalLayout.addLayout(self.verticalLayout_2)
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.pushButton_2 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_2.setObjectName("pushButton_2")
        self.verticalLayout.addWidget(self.pushButton_2)
        self.widget_2 = PlotCanvas(self.horizontalLayoutWidget)
        self.widget_2.setStyleSheet("background-color: rgb(255, 255, 255);")
        self.widget_2.setObjectName("widget_2")
        self.verticalLayout.addWidget(self.widget_2)
        self.horizontalLayout.addLayout(self.verticalLayout)
        self.verticalLayout_3 = QtWidgets.QVBoxLayout()
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.pushButton_3 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_3.setObjectName("pushButton_3")
        self.verticalLayout_3.addWidget(self.pushButton_3)
        self.widget_4 = PlotCanvas(self.horizontalLayoutWidget)
        self.verticalLayout_3.addWidget(self.widget_4)
        self.horizontalLayout.addLayout(self.verticalLayout_3)
        MainWindow.setCentralWidget(self.centralwidget)

        self.pushButton_3.clicked.connect(self.channel0and1)
        self.pushButton_2.clicked.connect(self.channel1)
        self.pushButton.clicked.connect(self.channel0)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.pushButton.setText(_translate("MainWindow", "Chanel0"))
        self.pushButton_2.setText(_translate("MainWindow", "Channel1"))
        self.pushButton_3.setText(_translate("MainWindow", "Channel0 and 1"))

    def channel0and1(self):
        self.widget_4.plot0and1()

    def channel1(self):
        self.widget_2.plot1()

    def channel0(self):
        self.widget_3.plot0()

class PlotCanvas(FigureCanvas):

    def __init__(self,parent = None,*args,**kwargs):
        fig = Figure()
        self.axes = fig.add_subplot(111)
        self.factor = kwargs.pop("factor", 2)
        FigureCanvas.__init__(self, fig, *args,**kwargs)
        self.setParent(parent)
        FigureCanvas.setSizePolicy(self,QtWidgets.QSizePolicy.Expanding,QtWidgets.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        self.setFocus()

    def GetSerialPort(self,chanel=0):
        ser = serial.Serial("COM4",115200,timeout=1)
        Bucket = []
        for i in range(0,200):
            x = (ser.readline().decode("UTF-8")).strip()
            Bucket.append(float(x))
            print(Bucket)
        return Bucket

    def forchannel0(self,Bucket):
        Channel0 = []
        i = 0
        while i < len(Bucket)-1:
            Channel0.append(Bucket[i])
            i = i+2
        return Channel0

    def forchannel1(self,Bucket):
        Channel1 = []
        i = 1
        while i < len(Bucket)-1:
            Channel1.append(Bucket[i])
            i = i+2
        return  Channel1

    def plot0(self):
        Bucket = self.GetSerialPort()
        ax = self.figure.add_subplot(111)
        Bucket0=self.forchannel0(Bucket)
        ax.plot(Bucket0)
        self.draw()
        ax.axis

#        ax = self.figure.add_subplot(111)
#        ax.plot(self.GetSerialPort())
#        self.draw()
#        ax.axis

    def plot1(self):
        Bucket = self.GetSerialPort()
        ax = self.figure.add_subplot(111)
        Bucket1=self.forchannel1(Bucket)
        ax.plot(Bucket1)
        self.draw()
        ax.axis
#        ax = self.figure.add_subplot(111)
#        ax.plot(self.GetSerialPort1())
#        self.draw()
#        ax.axis

    def plot0and1(self):
        Bucket = self.GetSerialPort()
        ax = self.figure.add_subplot(111)
        ax.plot(self.forchannel0(Bucket))
        ax.plot(self.forchannel1(Bucket))
        self.draw()
        ax.axis


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
