##Tester for Windows Low Level Devices APIs

This repository contains firmware source code for the MBED LPC1768 that
enables functional testing of the Windows low level devices APIs,
including Windows.Devices.I2c. Binary firmware images are included in the HLK
that can be copied to the MBED. This is the source code for that firmware.

###Build the code

 1. Download and install [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded).
    Choose the .exe installer in the list of downloads.

 1. Clone the [ms-iot/mbed-windows](https://github.com/ms-iot/mbed-windows.git)
    repository:
    
        git clone https://github.com/ms-iot/mbed-windows.git
    
 1. Clone this repository to the same parent directory as mbed-windows.
 
        git clone https://github.com/ms-iot/busses-tester.git
        
 1. Put the compilers and tools on your path

        cd mbed-windows
        setenv.cmd
        
 1. CD to this repository and build

        cd ../busses-tester
        nmake

 1. Copy `busses-tester.bin` to your MBED and hit the RESET button.
 
The MBED is now running the firmware and is ready for the HLK.

### I2C Test Device Protocol

This section documents the protocol expected by the Windows.Devices.I2c
extended functional tests. This protocol is implemented by the busses-tester.

The I2C test device has a register address space of 256 bytes. Some registers have
special functions that control the behavior of the device. The following table 
describes the behavior of these registers.

<table>
<thead>
<tr>
  <th>Address</th>
  <th>Register/Region Name</th>
  <th>Description</th>
  <th>Reset Value</th>
</tr>
</thead>
<tbody><tr>
  <td>0x0-0x7F</td>
  <td>EEPROM</td>
  <td>Writes to this area behaves like an EEPROM. The address pointer rolls over in this region.</td>
  <td>0x55</td>
</tr>
<tr>
  <td>0x80-0xF7</td>
  <td>RESERVED</td>
  <td>Writes to these registers are ignored. Reading from these registers returns 0x55.</td>
  <td>0x55</td>
</tr>
<tr>
  <td>0xF8</td>
  <td>DISABLE_REPEATED_STARTS</td>
  <td>This register controls whether repeated starts are enabled or disabled. Writing a 1 to this register disables repeated starts for the next operation, meaning the device will not ACK its own slave address until a stop condition has been received. Writing 0 to this register enables normal operation. Disabling repeated starts is a one-shot operation. Repeated starts will be enabled after the next IO operation and this register will be reset to 0.</td>
  <td>0x0</td>
</tr>
<tr>
  <td>0xF9</td>
  <td>SCL_HOLD_MILLIS_HI</td>
  <td>Contains the duration in milliseconds (Hi byte) for which SCL will be held low when either the hold read or hold write condition is triggered.</td>
  <td>0x3A</td>
</tr>
<tr>
  <td>0xFA</td>
  <td>SCL_HOLD_MILLIS_LO</td>
  <td>Contains the duration in milliseconds (Lo byte) for which SCL will be held low when either the hold read or hold write condition is triggered.</td>
  <td>0x98</td>
</tr>
<tr>
  <td>0xFB</td>
  <td>HOLD_READ_CONTROL</td>
  <td>Controls how many bytes of the next read the slave will transmit before pulling SCL low for the duration specified in SCL_HOLD_MILLIS_HI/LO. Writing a value other than 0xFF to this register arms hold read mode, where the register value is the number of bytes the slave will transmit before pulling SCL low. Writing a value of 0 will cause SCL to be pulled low immediately after the address byte is received. The device transmits sequentially increasing data in hold read mode (0, 1, 2, 3 ...). Hold read is a one-shot operation. After the next read operation, hold read mode is cleared and this register resets to 0xFF. Values: <br> 0x0 - 0xFE - hold read mode is armed for the next read<br>0xFF - hold read mode is not armed</td>
  <td>0xFF</td>
</tr>
<tr>
  <td>0xFC</td>
  <td>HOLD_WRITE_CONTROL</td>
  <td>Controls how many bytes of the next write the slave will ACK before pulling SCL low for the duration specified in SCL_HOLD_MILLIS_HI/LO. Writing a value other than 0xFF to this register arms hold write mode, where the register value is the number of bytes the slave will receive before pulling SCL low. Writing a value of 0 will cause SCL to be pulled low immediately after the address byte is received. Data transmitted by the master is ignored in hold write mode. Hold write is a one-shot operation. After the next write operation, hold write mode is cleared and this register resets to 0xFF. Values: <br> 0x0 - 0xFE - hold write mode is armed for the next write<br>0xFF - hold write mode is not armed</td>
  <td>0xFF</td>
</tr>
<tr>
  <td>0xFD</td>
  <td>NAK_CONTROL</td>
  <td>Controls how many bytes of the next write operation the slave will ACK before generating a NAK. Writing a value other than 0xFF to this register arms NAK mode, where the value written is the number of bytes the slave will ACK before NAKing a received byte. Data transmitted by the master is ignored in NAK mode. NAK mode is a one-shot operation. After the next write operation, NAK mode is cleared and this register resets to 0xFF. Values: <br>0x0 - 0xFE - The next write will be NAK'd after the number of bytes specified in this register <br>0xFF - NAK mode not armed</td>
  <td>0xFF</td>
</tr>
<tr>
  <td>0xFE</td>
  <td>CHECKSUM_UPDATE</td>
  <td>The device keeps a running CRC16 of all bytes written to this register. Writes to this register update the checksum. Reads from this register return the HI byte of the checksum. The LO byte of the checksum can be obtained by reading the CHECKSUM_RESET register.  Writes to this register do not affect the address pointer. Reads from this register increment the address pointer.</td>
  <td>0x0</td>
</tr>
<tr>
  <td>0xFF</td>
  <td>CHECKSUM_RESET</td>
  <td>Writes to this register clear the checksum to 0. Reads from this register return the LO byte of the 16-bit checksum. Writes to this register do not affect the address pointer. Reading this register increments the address pointer.</td>
  <td>0x0</td>
</tr>
</tbody></table>
