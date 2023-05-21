//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package fattreenew;

import fattreenew.StorageDevice;

simple flashBuffer extends Buffer
{
    @display("i=,#57E389");
}

network OST
{
    parameters:
        @display("i=device/drive;bgb=786,317");
        int num_devs = default(10);
    gates:
        input in;
        output out;
    submodules:
        storageDevice[num_devs]: StorageDevice {
            @display("p=437,86,c,20");
        }
        flashBuffer[num_devs]: flashBuffer {
            @display("p=253,86,c,20");
        }
        payloadOST: Payload {
            @display("p=102,167");
        }
    connections:
        in --> payloadOST.in++;
        for i=0..(num_devs-1) {
            payloadOST.port++ <--> flashBuffer[i].port++;
            flashBuffer[i].port++ <--> storageDevice[i].port++;
        }
        payloadOST.out++ --> out;
}
