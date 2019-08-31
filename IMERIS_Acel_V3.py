import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque
import matplotlib.pyplot as plt 
import matplotlib.animation as animation
import multiprocessing as mp
#import db
#from statistics import mean
#import pymysql
import datetime


ax1 = 0
ax2 = 0
ax3 = 0
ax4 = 0

cte = 32768/4096*3300
g = 32768/4096*660

#q = mp.Queue()

# plot class
class AnalogPlot:
  # constr
  def __init__(self, strPort, maxLen, maxLen2, date):
    # open serial port

    self.q = mp.Queue()
    self.q1 = mp.Queue()
    self.ser = serial.Serial(strPort, 115200)
    self.ser2 = serial.Serial("/dev/ttyACM0", 115200)
 
    self.maxLen = maxLen
    self.tiempo = deque([])
    self.ax = deque([])
    self.dx = deque([])  
    self.archivo = open(date, "w")

    #proceso que lee datos del USB
    sleep(1)
    #self.ser.reset_input_buffer()
    
    try:
      self.p1 = mp.Process(target=self.dataGen)
      #sefl.p1.daemon = True
      self.p1.start()
      print("p1 inicia")
    except:
      print ("Error: unable to start process p1")
    



  def dataGen(self):
    #print (self.ser.inWaiting())
    #sleep(0.1)
      
    while (self.ser2.inWaiting() == self.ser.inWaiting()):
        if (self.ser.inWaiting() > 10):
            line = self.ser.readline()
        if (self.ser2.inWaiting() > 10):
            line = self.ser2.readline()
    
    while (True):
      if (self.ser.inWaiting() > 10):
        print(self.ser.inWaiting(), self.ser2.inWaiting())
        #datetime_object = datetime.datetime.now() #- datetime.timedelta(milliseconds=4)*self.ser.inWaiting()
        line = self.ser.readline()
        line2 = self.ser2.readline()
        line = str(line)
        line2 = str(line2)
        #print(line)
        line = line[4:]
        line = line[:-3]
        line2 = line2[4:]
        line2 = line2[:-3]
        #line = line + "," + str(datetime_object.timestamp())
        #print((line+","+line2))
        self.q.put((line+","+line2))
        #print(line)
        
      
  # add to buffer
  def addToBuf(self, buf, val):
    if len(buf) < self.maxLen:
      buf.append(val)
      
    else:
      buf.popleft()
      buf.append(val)
      
  # add data
  def add(self, data):
    global cte
    global g
    assert(len(data) == 3)

    time = data[0]/1000
    x = (data[1]-cte/2)/(g)
    dx = data[2]

    string = str(time) + "," + str(x) + "," + str(dx) + "\n"
    self.archivo.write(string)

    self.addToBuf(self.ax, x)
    self.addToBuf(self.dx, dx)
    self.addToBuf(self.tiempo, time)
    
    
    
  # update plot
  def update(self, frameNum, a0, a4, a1, a5):
    global ax1
    global ax2
    global ax3
    global ax4
    try:
      while (self.q.qsize() > 10):
        data = self.q.get(block=True, timeout=.5)
#        print(line)
        #print(type(line))
#        print (data)
        try:
          
          data = [float(val) for val in data.split(",")]
          #print(data)
        except:
          print("Error en data")

        if(len(data) == 3):
          self.add(data)
          a0.set_data(self.tiempo, self.ax)
          a1.set_data(self.tiempo, self.dx)
                            
      try:
        ax1.set_xlim(min(self.tiempo), max(self.tiempo))
        ax3.set_xlim(min(self.tiempo), max(self.tiempo))
        ax3.set_ylim(min(self.dx)-0.05, max(self.dx)+0.05)
      except:
        pass
          
      try:
        #Fs = (len(self.tiempo))/(self.tiempo[0]-self.tiempo[(self.maxLen -1)])
        Fs = (len(self.tiempo))/(self.tiempo[-1]-self.tiempo[0])
        #print (Fs)
        Ts = 1.0/Fs
        n = len(self.ax)
        k = np.arange(n)
        T = n/Fs
        frq = k/T
        frq = frq[range(int(n/2))] 

        """
        fft_ax = np.fft.fft(self.ax)/n
        fft_ay = np.fft.fft(self.ay)/n
        fft_az = np.fft.fft(self.az)/n
        """
        fft_ax = np.fft.fft(self.ax)
        fft_dx = np.fft.fft(self.dx)
        
        fft_ax = fft_ax[range(int(n/2))]
        fft_dx = fft_dx[range(int(n/2))]
         
        #print (fft_ax[0], fft_ay[0], fft_az[0])

        fft_ax[0] = 0
        fft_dx[0] = 0
                  
        a4.set_data(frq, abs(fft_ax))
        a5.set_data(frq, abs(fft_dx))
        
        #print (abs(fft_ax[0]),abs(fft_ay[0]), abs(fft_az[0]))
          
        ax2.set_ylim(0, max(abs(fft_ax)+0.01))
        ax4.set_ylim(0, max(abs(fft_dx)+0.01))
      except:
        pass
          
         
    except KeyboardInterrupt:
      print('exiting')
          
      
    return a0, a4, a1, a5

  # clean up
  def close(self):
      # close serial
      self.ser.flush()
      self.ser.close()
      self.ser2.flush()
      self.ser2.close()
      self.p1.terminate()
      self.archivo.close()
      
# main() function
def main():
  
  global ax1
  global ax2
  global ax3
  global ax4
  # create parser
  parser = argparse.ArgumentParser(description="LDR serial")
  # add expected arguments
  parser.add_argument('--port', dest='port', required=True)

  # parse args
  args = parser.parse_args()
  
  #strPort = '/dev/tty.usbserial-A7006Yqh'
  strPort = args.port

  print('reading from serial port %s...' % strPort)

  maxLeng = 1000
  maxLeng2 = 2000
  #date = f"{datetime.datetime.now():%Y%m%d_%H%M%S}" + ".txt"
  date = str(datetime.datetime.now())
  date = date + ".txt"
  # plot parameters
  analogPlot = AnalogPlot(strPort, maxLeng, maxLeng2, date)
  

  print('plotting data...')

  # set up animation
  fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1)
  
  a0, = ax1.plot([], [], label = "ax")
  
  a1, = ax3.plot([], [], label = "ax_ef")
  
  a4, = ax2.plot([], [], label = "ax_ef")
  
  a5, = ax4.plot([], [], label = "ax_ef")
  
  ax = [a0, a4, a1, a5]

  ax1.set_ylim(-1.5, 1.5)
  ax3.set_ylim(-75, 75)
  ax2.set_ylim(0, 50)
  ax2.set_xlim(0, 64)
  ax4.set_ylim(0, 50)
  ax4.set_xlim(0, 64) 
  
  try: 
    anim = animation.FuncAnimation(fig, analogPlot.update, 
                                 fargs=(a0, a4, a1, a5,), 
                                 interval=100)

    # show plot
    plt.legend(loc="upper right")
    plt.show()
  
    # clean up
  except:
    pass

  analogPlot.close()

  print('exiting.')

  

# call main
if __name__ == '__main__':
  main()
