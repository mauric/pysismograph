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
#plt.style.use('fivethirtyeight')   
plt.style.use('seaborn')  #TODO CHANGE STYLE 

#-----------------------------------#
ax1 = 0
ax2 = 0

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
 
    self.maxLen = maxLen
    self.tiempo = deque([])
    self.ax = deque([])    
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
    while (True):
      if (self.ser.inWaiting() > 5):
        #datetime_object = datetime.datetime.now() #- datetime.timedelta(milliseconds=4)*self.ser.inWaiting()
        line = self.ser.readline()
        line = str(line)
        line = line[4:]
        line = line[:-3]
        #line = line + "," + str(datetime_object.timestamp())
        self.q.put(line)
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
    assert(len(data) == 2)

    time = data[0]/1000
    x = (data[1]-cte/2)/(g)

    string = str(time) + "," + str(x) + "\n"
    self.archivo.write(string)

    self.addToBuf(self.ax, x)
    self.addToBuf(self.tiempo, time)
    
    
    
  # update plot
  def update(self, frameNum, a0, a4):
    global ax1
    global ax2
    try:
      while (self.q.qsize() > 10):
        data = self.q.get(block=True, timeout=.5)
        #print(line)
        #print(type(line))
        #print (data)
        try:
          
          data = [int(val) for val in data.split(",")]
          #print(data)
        except:
          print("Error en data")

        if(len(data) == 2):
          self.add(data)
          a0.set_data(self.tiempo, self.ax)
                            
      try:
        ax1.set_xlim(min(self.tiempo), max(self.tiempo))
        #ax1.set_ylim(min(min(self.ax), min(self.ay), min(self.az))-0.05, max(max(self.ax), max(self.ay), max(self.az))+0.05)
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
        
        fft_ax = fft_ax[range(int(n/2))]
         
        #print (fft_ax[0], fft_ay[0], fft_az[0])

        fft_ax[0] = 0
                  
        a4.set_data(frq, abs(fft_ax))
        
        #print (abs(fft_ax[0]),abs(fft_ay[0]), abs(fft_az[0]))
          
        ax2.set_ylim(0, max(abs(fft_ax)+0.01))
      except:
        pass
          
         
    except KeyboardInterrupt:
      print('exiting')
          
      
    return a0, a4

  # clean up
  def close(self):
      # close serial
      self.ser.flush()
      self.ser.close()
      self.p1.terminate()
      self.archivo.close()
      
# main() function
def main():
  
  global ax1
  global ax2
  # create parser
  parser = argparse.ArgumentParser(description="LDR serial")
  # add expected arguments
  parser.add_argument('--port', dest='port', required=True)

  # parse args
  args = parser.parse_args()
  
  #strPort = '/dev/tty.usbserial-A7006Yqh'
  #strPort = '/dev/ttyUSB0'
  strPort = args.port

  print('reading from serial port %s...' % strPort)

  maxLeng = 1000
  maxLeng2 = 2000
  date = f"{datetime.datetime.now():%Y%m%d_%H%M%S}" + ".txt"

  # plot parameters
  analogPlot = AnalogPlot(strPort, maxLeng, maxLeng2, date)
  

  print('plotting data...')

  # set up animation
  fig, (ax1, ax2) = plt.subplots(2, 1)
  
  a0, = ax1.plot([], [])
  a4, = ax2.plot([], [])

  ax = [a0, a4]

  ax1.set_title('Aceleracion de la mesa')
  ax1.set_xlabel('tiempo [s]')
  ax1.set_ylabel('m/s')
  fig.suptitle('IMERIS Mesvib -live-', fontsize=16)
  ax1.set_ylim(-1.5, 1.5)
  ax1.grid(color='b', linestyle='-', linewidth=0.25)

  ax2.set_title('Frecuencia')
  ax2.set_xlabel('Hz')
  ax2.set_ylabel('|F(s)|')

  ax2.grid(color='b', linestyle='-', linewidth=0.25)
  ax2.set_ylim(0, 50)
  ax2.set_xlim(0, 125) 
 #

  try: 
   anim = animation.FuncAnimation(fig, analogPlot.update, 
                                 fargs=(a0, a4,), 
                                 interval=100)
  
   # show plot
   plt.show()
 
   # clean up
  except:
  	pass

  analogPlot.close()

  print('exiting.')

  

# call main
if __name__ == '__main__':
  main()
