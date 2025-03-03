==Description==
Cranfield University, 2025 NB-IoT satellite constellation Group Design Project (GDP), 
On Board Data Handling (OBDH) program, Telemetry, Tracking, and Command (TT&C) basecamp cFS application and payload program.

==Requirements==
- cmake (>v3.10)
- g++
- git
- linux environnement (or VM, WSL)

==Optional==
- Doxygen (for documentation generation)

==Common installation==

Install required packages,
<pre>
sudo apt install cmake build-essential doxygen git
</pre>

Clone from the github repository,
<pre>
cd ~
git clone https://github.com/squaria1/OBDH_Program.git
</pre>

==OBDH program installation==
Build the OBDH program,
<pre>
cd OBDH_Program
mkdir build
cd build
cmake -S ../ -B .
make
</pre>

Run the OBDH program,
<pre>
sudo ./OBDH_Program
</pre>

(Optional) Generate OBDH program documentation with Doxygen,
<pre>
cd ../../doc
doxygen Doxyfile
</pre>

Then open html/index.html with a web browser.

==Payload program installation==
Build the Payload program,
<pre>
cd ~
cd OBDH_Program/Payload_Program
mkdir build
cd build
cmake -S ../ -B .
make
</pre>

Run the Payload program,
<pre>
sudo ./OBDH_Program
</pre>

(Optional) Generate Payload program documentation with Doxygen,
<pre>
cd ../doc
doxygen Doxyfile
</pre>

Then open html/index.html with a web browser.

==cFS basecamp TT&C application installation==
Follow cFS basecamp installation instructions on https://github.com/cfs-tools/cfs-basecamp,

Run the basecamp GUI with (where env/ and cfs-basecamp/ directories are located),
<pre>
source env/bin/activate
cd cfs-basecamp/gnd-sys/app/
. ./setvars.sh
python3 basecamp.py
</pre>

Create the hi_world tutorial application with the basecamp GUI,
File-->Create user app-->Toggle Hello World-->Create App

Add the hi_world app to the cFS build configuration,
File-->Add User App to Target-->Select "hi_world" in the dropdown-->Click on "Auto"-->Close window

Replace,
<pre>
"APP_CMD_PIPE_DEPTH": 5
</pre>
In "cfs-basecamp/cfe-eds-framework/basecamp_defs/cpu1_hi_world_ini.json"
By,
<pre>
"APP_CMD_PIPE_MAX": 7
</pre>

Close basecamp,
File-->Exit

Remove generated basecamp hi_world/ and copy hi_world/ from the git to the basecamp installation,
<pre>
rm -R <<CHANGE TO YOUR BASECAMP ENV DIRECTORY>>/cfs-basecamp/usr/apps/hi_world
cp -r ~/OBDH_Program/basecamp_cFS_custom_app/hi_world <<CHANGE TO YOUR BASECAMP ENV DIRECTORY>>/cfs-basecamp/usr/apps/
</pre>

Start basecamp again,
<pre>
python3 basecamp.py
</pre>

Build the application,
Click on "Build New"

Start the application,
Click on "Start"

Send TM to the OBDH Program or payload Program (send to the OBDH and rerouted to the payload program through the CAN bus),
"Send Cmd" dropdown-->Select HI_WORLD/Application/CMD-->Select "SetParam" in the dropdown-->Select main state to change (only one at a time)
