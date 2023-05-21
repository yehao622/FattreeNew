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
        input in;
        output out;
    submodules:
        oss_memory: dramBuffer {
            @display("p=223,191");
        }

        ost[num_osts]: OST {
            @display("p=338,372");
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

        oss_out_payload: Payload {
            @display("p=68,191");
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

    connections:
        in --> oss_in_payload.in++;

        oss_in_payload.out++ --> oss_hub_mem_hca.in++;
        oss_memory.port++ <--> oss_hub_mem_hca.port++;
        oss_memory.port++ <--> oss_hub_mem_hba.port++;

        for i=0..(num_hcas-1) {
            oss_hub_mem_hca.out++ --> hca[i].in;
            hca[i].out --> oss_hub_mem_hca.in++;
        }

        for i=0..(num_hbas-1) {
            oss_hub_mem_hba.out++ --> hba[i].in;
            hba[i].out --> oss_hub_hba_ost.in++;
        }

        for i=0..(num_osts-1) {
            oss_hub_hba_ost.out++ --> ost[i].in;
            ost[i].out --> oss_hub_hba_ost.in++;
        }

        oss_hub_mem_hca.out++ --> oss_out_payload.in++;
        oss_out_payload.out++ --> out;
        oss_hub_hba_ost.out++ --> oss_hub_mem_hba.in++;
        oss_hub_hba_ost.port++ <--> oss_memory.port++;
}
