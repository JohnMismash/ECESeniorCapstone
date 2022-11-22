import threading
import time 

class MyThread(threading.Thread):
    """Thread class with a stop() method. The thread itself has to check
    regularly for the stopped() condition."""
    def __init__(self, *args, **kwargs):
        super(MyThread, self).__init__(*args, **kwargs)
        self._stop = threading.Event()

    def stop(self):
        self._stop.set()

    def stopped(self):
        return self._stop.isSet()


class MyClass :

    def clicked_practice(self):

        self.practicethread = MyThread(target=self.infinite_loop_method)
        self.practicethread.start()

    def infinite_loop_method(self):
        #self.practicethread.start()
        while not self.practicethread.stopped():
           print("Running Still")


    #This doesn't seem to work and I am still stuck in the loop
    def infinite_stop(self):
        self.practicethread.stop()



c = MyClass()
c.clicked_practice() 
print("I have control")
time.sleep(5)
c.infinite_stop()


#c.infinite_stop()

