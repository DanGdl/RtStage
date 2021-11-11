
Program writer reads a file splits it on 100 byte chuncks and sends it via serial device.
Program reader reads chunck from serial device, orders them and saves in file

Compile project wia make all or make writer, make reader

Create virtual serial device via socat: socat pty,raw,echo=0 pty,raw,echo=0

Launch programs
./reader path_to_tty_to_read_from path_to_file_to_store_all_data 

./writer path_to_file_to_read path_to_tty_to_send_there_data
