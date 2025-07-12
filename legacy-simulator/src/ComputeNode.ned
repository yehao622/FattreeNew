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

import fattreenew.HCA;
import fattreenew.Buffer;
import fattreenew.WorkGenerator;
import fattreenew.PCIe;

network ComputeNode
{
    parameters:
        @display("i=device/mainframe;bgb=607,343");
        int num_hca = default(4);
    gates:
        inout port[];
    submodules:
        cn_memory: Buffer {
            @display("p=189,160;i=,#5E5C64");
        }
        hca[num_hca]: HCA {
            @display("p=490,160");
        }
        work_gen: WorkGenerator {
            @display("p=66,160");
        }
        cn_memory_hca: Payload {
            @display("p=369,160");
        }
        pci: PCIe {
            @display("p=280,160");
        }
    connections allowunconnected:
        port++ <--> cn_memory_hca.port++;
        work_gen.port <--> cn_memory.port++;
        cn_memory.port++ <--> pci.port++;
        pci.port++ <--> cn_memory_hca.port++;
        for i=0..(num_hca-1) {
            cn_memory_hca.port++ <--> hca[i].port++;
        }
}
