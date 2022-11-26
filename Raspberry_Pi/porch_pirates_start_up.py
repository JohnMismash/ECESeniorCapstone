#!/usr/bin/python3

import traceback
import runpy
import urllib.request

connect = False

while connect is False:
    try: 
        urllib.request.urlopen('https://google.com')
        
        connect = True
        print("Can Connect To Internet")
        
    except:
        pass


path_main_program = "/home/username/porchPirates/ECESeniorCapstone/Raspberry_Pi/mainProgram.py"

result = False

print("Attemtping to Run Program")

while result is False:
    try:
        #cd path_main_program
        #python3 mainProgram.py
        runpy.run_path(path_name = path_main_program)
        result = True
        
    except Exception:
        traceback.print_exc()
        result = True

