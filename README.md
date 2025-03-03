Description
-----------
Cranfield University, 2025 NB-IoT satellite constellation Group Design Project (GDP), 
On Board Data Handling (OBDH) program, Telemetry, Tracking, and Command (TT&C) basecamp cFS application and payload program, Electrical Power System (EPS) board KiCad project.


EPS board KiCad project
-----------------------
The EPS board is made from the fusion of two existing open source projects from https://libre.solar/:
* The MPPT: https://github.com/LibreSolar/mppt-2420-hc
* The BMS: https://github.com/LibreSolar/bms-8s50-ic

See https://learn.libre.solar/ for the documentation and schematic explanation.

Notable changes from the two source projects are:
* Complete board rerouting
* Add fuse to MPPT input
* Changed DC-DC 12V circuit
* BMS output into MPPT battery input
* Separate BMS and MPPT ground planes


OBDH and payload programs requirements
------------
* cmake (>v3.10)
* g++
* git
* linux environnement (or VM, WSL)


Optional
--------
* Doxygen (for documentation generation)


Common installation
-------------------
Install required packages,
```
sudo apt install cmake build-essential doxygen git
```

Clone from the github repository,
```
cd ~
git clone https://github.com/squaria1/OBDH_Program.git
```


OBDH program installation
-------------------------
Build the OBDH program,
```
cd ~/OBDH_Program
mkdir build
cd build
cmake -S ../ -B .
make
```

Run the OBDH program,
```
sudo ./OBDH_Program
```

(Optional) Generate OBDH program documentation with Doxygen,
```
cd ../../doc
doxygen Doxyfile
```

Then open html/index.html with a web browser.


Payload program installation
----------------------------
Build the Payload program,
```
cd ~/OBDH_Program/Payload_Program
mkdir build
cd build
cmake -S ../ -B .
make
```

Run the Payload program,
```
sudo ./OBDH_Program
```

(Optional) Generate Payload program documentation with Doxygen,
```
cd ../doc
doxygen Doxyfile
```

Then open html/index.html with a web browser.


cFS basecamp TT&C application installation
------------------------------------------
Follow cFS basecamp installation instructions on https://github.com/cfs-tools/cfs-basecamp,

Run the basecamp GUI with (where env/ and cfs-basecamp/ directories are located),
```
source env/bin/activate
cd cfs-basecamp/gnd-sys/app/
. ./setvars.sh
python3 basecamp.py
```

Create the hi_world tutorial application with the basecamp GUI,
File-->Create user app-->Toggle Hello World-->Create App

Add the hi_world app to the cFS build configuration,
File-->Add User App to Target-->Select "hi_world" in the dropdown-->Click on "Auto"-->Close window

Replace,
```
"APP_CMD_PIPE_DEPTH": 5
```
In "cfs-basecamp/cfe-eds-framework/basecamp_defs/cpu1_hi_world_ini.json"
By,
```
"APP_CMD_PIPE_MAX": 7
```

Close basecamp,
File-->Exit

Remove generated basecamp hi_world/ and copy hi_world/ from the git to the basecamp installation,
```
rm -R <<CHANGE TO YOUR BASECAMP ENV DIRECTORY>>/cfs-basecamp/usr/apps/hi_world
cp -r ~/OBDH_Program/basecamp_cFS_custom_app/hi_world <<CHANGE TO YOUR BASECAMP ENV DIRECTORY>>/cfs-basecamp/usr/apps/
```

Start basecamp again,
```
python3 basecamp.py
```

Build the application,
Click on "Build New"



Start the application,
Click on "Start"

Send telecommands (TCs) to the OBDH Program or payload Program (send to the OBDH and rerouted to the payload program through the CAN bus),
"Send Cmd" dropdown-->Select HI_WORLD/Application/CMD-->Select "SetParam" in the dropdown-->Select main state to change (only one at a time)

Show recieved telemetry (TM),
"View Tlm" dropdown-->Select HI_WORLD/Application/STATUS_TLM
