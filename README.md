# Home Irrigation Server
The Linux server program of the home irrigation system, running on Raspberry Pi 3B+<br>

## System Requirements
Raspberry Pi model: 3B+<br>
Operating System: Raspberry Pi OS<br>

## Dependencies
pigpio, openssl and sqlite library<br>
How to install the dependencies:<br>
sudo apt install pigpio<br>
sudo apt install openssl libssl-dev<br>
sudo apt install sqlite3<br>

## How to compile and run
In this directory, execute "make", and after the make process finished, go to bin directory (use the command "cd bin"), and run home_irrigation_server executable file as root.<br>