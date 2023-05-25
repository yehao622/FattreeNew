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

import fattreenew.OST;
import fattreenew.HCA;
import fattreenew.HBA;
import fattreenew.PCIe;
import fattreenew.simulations.dramBuffer;
import ned.IdealChannel;


network OSS
{
    parameters:
        @display("i=device/server;bgb=620,445");
        int num_hcas = default(2);
        int num_hbas = default(2);
        int num_osts = default(10);
    gates:
        inout port[];
    submodules:
        oss_memory: dramBuffer {
            @display("p=223,191");
        }

        ost[num_osts]: OST {
            @display("p=338,406");
        }

        hca[num_hcas]: HCA {
            @display("p=472,81");
        }

        hba[num_hbas]: HBA {
            @display("p=472,191");
        }

        oss_in_payload: Payload {
            @display("p=58,81");
        }

        oss_hub_mem_hca: Payload {
            @display("p=223,81");
        }

        oss_hub_mem_hba: Payload {
            @display("p=338,191");
        }

        oss_hub_hba_ost: Payload {
            @display("p=338,290");
        }
        pci: PCIe {
            @display("p=223,138");
        }
        sas[num_osts]: SAS {
            @display("p=338,352");
        }
    connections:
        port++ <--> oss_in_payload.port++;

        oss_in_payload.port++ <--> oss_hub_mem_hca.port++;
        oss_hub_mem_hca.port++ <--> pci.port++;
        oss_memory.port++ <--> pci.port++;
        oss_memory.port++ <--> oss_hub_mem_hba.port++;

        for i=0..(num_hcas-1) {
            oss_hub_mem_hca.port++ <--> hca[i].port++;
        }

        for i=0..(num_hbas-1) {
            oss_hub_mem_hba.out++ --> hba[i].in;
            hba[i].out --> oss_hub_hba_ost.in++;
        }

        for i=0..(num_osts-1) {
            oss_hub_hba_ost.port++ <--> sas[i].port++;
            sas[i].port++ <--> ost[i].port++;
        }

        oss_hub_hba_ost.out++ --> oss_hub_mem_hba.in++;
        oss_hub_hba_ost.port++ <--> oss_memory.port++;
}
